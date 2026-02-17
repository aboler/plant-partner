// Standard C Libraries
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// ESP-IDF Libraries
#include "esp_log.h"

// Data Types
#include "../dataTypes/plantData.h"

// Peripherals
#include "../peripherals/adc.h"
#include "../peripherals/gpio.h"
#include "../peripherals/heartbeat.h"
#include "../peripherals/pwm_pump.h"
#include "../peripherals/communication/http.h"
#include "../peripherals/communication/mqtt.h"
#include "../peripherals/communication/wifi.h"

// Debug options utilized for confirming performance
// static struct plantData pv1 = {"Sunflower",1,2,3,4,5};
const static char *TAG = "DEBUG";

// Set up networking (all Networking depends on nvs_init())
static esp_err_t nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

void app_main(void)
{
    // Used by all
    ESP_ERROR_CHECK(nvs_init());

    // Declare variables
    int adc_raw, voltage;
    bool auto_care_on, light_calibration_successful, moisture_calibration_successful;
    auto_care_on = false;
    const char *message;
    const char *topic;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t light_cali_adc1_handle, moisture_cali_adc1_handle;
    light_cali_adc1_handle = moisture_cali_adc1_handle = NULL;

    struct plantData p = {"Sunflower", 1, 2, 3, 4, 5};
    struct plantData *p_ptr = &p;


    // Start up peripheral communication needed to interface with the database
    start_wifi();
    mqtt_app_start();
    esp_http_client_handle_t client;
    client = http_configure_handle();
    http_put_plant_data(client, p_ptr);

    // Initialize ADC for photoresistor and moisture sensor
    light_calibration_successful = adc_init(&adc1_handle, LIGHT, &light_cali_adc1_handle);
    moisture_calibration_successful = adc_init(&adc1_handle, MOISTURE, &moisture_cali_adc1_handle);

    // Configure LEDs
    heartbeat_init();
    configure_IO(OUTPUT, EXTERNAL_LED_GPIO);
    clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

    // Configure PWMs
    pwm_pump_init(WATER);
    pwm_pump_init(FERTLIZER);

    while (1)
    {
        // Yield until MQTT sends message
        if (mqtt_check_buffer_ready())
        {
            // Grab MQTT topic and data
            message = read_data();
            topic = read_topic();

            ESP_LOGI("main", "Topic: %s, Data: %s", topic, message);

            // Toggle autocare enable command
            if (strcmp(topic, "plant_partner/act_tog_en") == 0)
            {
                if (strcmp(message, "Autocare enabled") == 0)
                    auto_care_on = true;
                else    
                    auto_care_on = false;

                ESP_LOGI(TAG, "Toggle autocare to: %d", auto_care_on);
            }
            // Sample command
            else if (strcmp(topic, "plant_partner/ack") == 0)
            {
                // 1. Assess and store light if configured
                if (light_calibration_successful)
                {
                    adc_read(LIGHT, adc1_handle, &adc_raw);
                    adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);

                    if (voltage < 0)
                    {
                        ESP_LOGW(TAG, "Invalid light reading", voltage);
                    }
                    else
                    {
                        p_ptr->lightIntensity = voltage;
                    }
                }

                // 2. Assess and store moisture if configured
                if (moisture_calibration_successful)
                {
                    adc_read(MOISTURE, adc1_handle, &adc_raw);
                    adc_rawToVoltage(moisture_cali_adc1_handle, adc_raw, &voltage);

                    if (voltage < 0)
                    {
                        ESP_LOGW(TAG, "Invalid moisture reading", voltage);
                    }
                    else
                    {
                        p_ptr->soilMoisture = voltage;
                    }
                }

                // 3. Assess and store nutrient data

                // 4. Actuate if auto_schedule is on AND if needed
                if (auto_care_on)
                {
                    // Light
                    if (p_ptr->lightIntensity < LED_THRESHOLD)
                    {
                        set_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);
                        ESP_LOGI(TAG, "LED ON: Voltage %d mV is below threshold", p_ptr->lightIntensity);
                    }
                    else
                    {
                        clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);
                        ESP_LOGI(TAG, "LED OFF: Voltage %d mV is above threshold", p_ptr->lightIntensity);
                    }

                    // Moisture
                    if(p_ptr->soilMoisture < 1500)
                        ESP_LOGI(TAG, "WET: ADC%d Channel[%d] Showing How Wet: %d ", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);
                    else
                    {
                        ESP_LOGI(TAG, "DRY: ADC%d Channel[%d] Showing How Wet: %d ", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);

                        // Actuate water pump
                        modify_pump_duty_cycle(WATER, PWM_DUTY_100_PERCENT);
                        vTaskDelay(pdMS_TO_TICKS(800));
                        modify_pump_duty_cycle(WATER, 0);
                    }

                    // Fertilizer
                    // TBD: PUT THIS IN IF STATEMENT LIKE ^^ TO REACT BASED ON READINGS
                    modify_pump_duty_cycle(FERTLIZER, PWM_DUTY_100_PERCENT);
                    vTaskDelay(pdMS_TO_TICKS(800));
                    modify_pump_duty_cycle(FERTLIZER, 0);
                }

                // 5. Send data to database
                http_put_plant_data(client, p_ptr);
                ESP_LOGI(TAG, "HTTP request...");
                esp_err_t err = esp_http_client_perform(client);
                ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));
                ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", p_ptr->lightIntensity, p_ptr->soilMoisture);
            }
        }

        // Must be at end of while loop, allows other CPU to activate
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
