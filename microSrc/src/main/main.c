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
#include "../peripherals/communication/ringbuf.h"

#define MAX_TRANSMISSION_ATTEMPTS            5
const static char *TAG = "DEBUG";

const static char *TOPIC_AUTO_NOTIF = "plant_partner/auto_notif";
const static char *TOPIC_ACT_COMPLETE = "plant_partner/act_compl";
const static char *TOPIC_CHECK_TOGGLE = "plant_partner/act_tog_en";
const static char *ACTIVATION_MESSAGE_AUTOCARE = "Autocare toggled";
const static char *MESSAGE_AUTOCARE_ON = "Autocare ON";
const static char *MESSAGE_AUTOCARE_OFF = "Autocare OFF";
const static char *MESSAGE_WATER_DONE = "Water Complete";
const static char *MESSAGE_LIGHT_DONE = "Light Complete";
const static char *MESSAGE_NUTRI_DONE = "Nutrients Complete";







void app_main(void)
{
    // Declare variables
    int adc_raw, voltage;
    bool auto_care_on, light_calibration_successful, moisture_calibration_successful;
    auto_care_on = false;
    const char *message;
    const char *topic;
    esp_err_t err;

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
            //publish_mqtt("plant_partner/just_looped", "just looped");
            ESP_LOGI("main", "Topic: %s, Data: %s", topic, message);

            // Toggle autocare enable command
            if (strcmp(topic, TOPIC_CHECK_TOGGLE) == 0 && strcmp(message, ACTIVATION_MESSAGE_AUTOCARE) == 0)
            {   
                //Toggle effect
                    auto_care_on = !auto_care_on;

                    if(auto_care_on == true)
                        publish_mqtt(TOPIC_AUTO_NOTIF , MESSAGE_AUTOCARE_ON);
                    else
                        publish_mqtt(TOPIC_AUTO_NOTIF, MESSAGE_AUTOCARE_OFF);
                


                //ESP_LOGI(TAG, "Toggle autocare to: %d", auto_care_on);*/
            }
            // Sample command
            else if (strcmp(topic, "plant_partner/ack") == 0)
            {
                // If want to just control water
                if(strcmp(message, "water") == 0)
                {
                    modify_pump_duty_cycle(WATER, PWM_DUTY_100_PERCENT);
                    vTaskDelay(pdMS_TO_TICKS(300));
                    modify_pump_duty_cycle(WATER, 0);

                    // Update measured value
                    if (moisture_calibration_successful)
                    {
                        adc_read(WATER, adc1_handle, &adc_raw);
                        adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);

                        if (voltage < 0)
                        {
                            ESP_LOGW(TAG, "Invalid moisture reading", voltage);
                        }
                        else
                        {
                            p_ptr->soilMoisture = voltage;
                        }
                    }

                    ESP_LOGI(TAG, "Water toggled: %d", p_ptr->soilMoisture);

                    // Send data to database
                    http_put_plant_data(client, p_ptr);
                    ESP_LOGI(TAG, "HTTP request...");
                    for (uint8_t try_count = 0; try_count < MAX_TRANSMISSION_ATTEMPTS; try_count++)
                    {
                        err = esp_http_client_perform(client);

                        if (err == ESP_OK)
                            break;
                    }
                    ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));
                    ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", p_ptr->lightIntensity, p_ptr->soilMoisture);

                    // Confirm the actuation was completed
                    publish_mqtt(TOPIC_ACT_COMPLETE, MESSAGE_WATER_DONE);
                }

                // If want to just control light
                else if(strcmp(message, "light") == 0)
                {
                    toggle_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Update measured value
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

                    ESP_LOGI(TAG, "LED toggled: %d", p_ptr->lightIntensity);

                    // Send data to database
                    http_put_plant_data(client, p_ptr);
                    ESP_LOGI(TAG, "HTTP request...");
                    for (uint8_t try_count = 0; try_count < MAX_TRANSMISSION_ATTEMPTS; try_count++)
                    {
                        err = esp_http_client_perform(client);

                        if (err == ESP_OK)
                            break;
                    }
                    ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));
                    ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", p_ptr->lightIntensity, p_ptr->soilMoisture);

                    // Confirm the actuation was completed
                    publish_mqtt(TOPIC_ACT_COMPLETE, MESSAGE_LIGHT_DONE);
                }

                // If want to just control fertilizer
                else if (strcmp(message, "nutrients") == 0)
                {
                    modify_pump_duty_cycle(FERTLIZER, PWM_DUTY_100_PERCENT);
                    vTaskDelay(pdMS_TO_TICKS(300));
                    modify_pump_duty_cycle(FERTLIZER, 0);

                    ESP_LOGI(TAG, "Fertilizer toggled:");

                    // Confirm the actuation was completed
                    publish_mqtt(TOPIC_ACT_COMPLETE, MESSAGE_NUTRI_DONE);
                }
                // Otherwise do default action 
                else
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
                        if (p_ptr->soilMoisture < MOISTURE_THRESHOLD)
                            ESP_LOGI(TAG, "WET: ADC%d Channel[%d] Showing How Wet: %d ", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);
                        else
                        {
                            ESP_LOGI(TAG, "DRY: ADC%d Channel[%d] Showing How Wet: %d ", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);

                            // Actuate water pump
                            modify_pump_duty_cycle(WATER, PWM_DUTY_100_PERCENT);
                            vTaskDelay(pdMS_TO_TICKS(300));
                            modify_pump_duty_cycle(WATER, 0);
                        }

                        // Fertilizer
                        // TBD: PUT THIS IN IF STATEMENT LIKE ^^ TO REACT BASED ON READINGS
                        modify_pump_duty_cycle(FERTLIZER, PWM_DUTY_100_PERCENT);
                        vTaskDelay(pdMS_TO_TICKS(300));
                        modify_pump_duty_cycle(FERTLIZER, 0);
                    }

                    // 5. Send data to database
                    http_put_plant_data(client, p_ptr);
                    ESP_LOGI(TAG, "HTTP request...");
                    for (uint8_t try_count = 0; try_count < MAX_TRANSMISSION_ATTEMPTS; try_count++)
                    {
                        err = esp_http_client_perform(client);

                        if (err == ESP_OK)
                            break;
                    }
                    ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));
                    ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", p_ptr->lightIntensity, p_ptr->soilMoisture);
                }    
            }
            else{
                
                ESP_LOGI(TAG, "MESSAGE ERROR");
            }
        }

        // Must be at end of while loop, allows other CPU to activate
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
