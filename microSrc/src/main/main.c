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

#define TOPIC_BACKEND_ACK "plant_partner/ack_back"
#define TOPIC_ESP32_ACK   "plant_partner/ack_esp32"
#define TOPIC_BACKEND_TOGGLE_AUTO_CARE "plant_partner/act_tog_en"
#define TOPIC_BACKEND_ADC_COMMAND "plant_partner/adc_command"
#define TOPIC_BACKEND_REQUEST_DATA "plant_partner/data_command"
#define TOPIC_ESP32_DATA          "plant_partner/esp32_status"
#define TOPIC_ESP32_AUTOCARE_ACTIVE "plant_partner/esp32_autocare_active"
#define TOPIC_ESP32_CURRENT_ACTION "plant_partner/current_state"






#define DEBUG_MODE_MAIN

//Test suites if Debug_Mode is flashed
#ifdef DEBUG_MODE_MAIN
    #define MQTT_CONNECTION_SUITE
#endif

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
    #ifndef DEBUG_MODE_MAIN
    heartbeat_init();
    #endif

    configure_IO(OUTPUT, EXTERNAL_LED_GPIO);
    clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

    // Configure PWMs
    pwm_pump_init(WATER);
    pwm_pump_init(FERTLIZER);
    bool connected = 0;
    while (1)
    { 
        // Yield until MQTT sends message
        if (mqtt_check_buffer_ready())
        {
            // Grab MQTT topic and data
            message = read_data();
            topic = read_topic();

            ESP_LOGI("main", "Topic: %s, Data: %s", topic, message);

            #ifdef MQTT_CONNECTION_SUITE
            // handshake
            if(!connected)
            {
                if (strcmp(topic, TOPIC_BACKEND_ACK) == 0)
                {
                    ESP_LOGI(TAG, "Backend ACK received - establishing connection");
                    publish_mqtt(TOPIC_ESP32_ACK, "ack_from_esp32");
                    connected = 1;
                    ESP_LOGI(TAG, "Connection established - ESP32 ready for commands");
                }
            }
            
            // check connected then check commands
            if(connected)
            {
                // toggle auto-care mode
                if (strcmp(topic, TOPIC_BACKEND_TOGGLE_AUTO_CARE) == 0)
                {
                    auto_care_on = !auto_care_on;
                    ESP_LOGI(TAG, "Auto-care toggled to: %s", auto_care_on ? "ENABLED" : "DISABLED");
                    
                    // publish status back to backend
                    char status_msg[32];
                    snprintf(status_msg, sizeof(status_msg), "auto_care:%d", auto_care_on);
                    publish_mqtt(TOPIC_ESP32_AUTOCARE_ACTIVE, status_msg);
                }
                
                // ADC sensor reading command
                else if (strcmp(topic, TOPIC_BACKEND_ADC_COMMAND) == 0)
                {
                    ESP_LOGI(TAG, "ADC command received - reading sensors");
                    char status_msg[32];
                    snprintf(status_msg, sizeof(status_msg), "ADC READING...,");
                    publish_mqtt(TOPIC_ESP32_CURRENT_ACTION, status_msg);
                    // read light sensor
                    if (light_calibration_successful)
                    {
                        adc_read(LIGHT, adc1_handle, &adc_raw);
                        adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);
                        
                        if (voltage < 0)
                        {
                            ESP_LOGW(TAG, "Invalid light reading: %d mV", voltage);
                        }
                        else
                        {
                            p_ptr->lightIntensity = voltage;
                            ESP_LOGI(TAG, "Light sensor: %d mV (raw: %d)", voltage, adc_raw);
                        }
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Light sensor not calibrated - skipping");
                    }

                    // read moisture sensor
                    if (moisture_calibration_successful)
                    {
                        adc_read(MOISTURE, adc1_handle, &adc_raw);
                        adc_rawToVoltage(moisture_cali_adc1_handle, adc_raw, &voltage);
                        
                        if (voltage < 0)
                        {
                            ESP_LOGW(TAG, "Invalid moisture reading: %d mV", voltage);
                        }
                        else
                        {
                            p_ptr->soilMoisture = voltage;
                            ESP_LOGI(TAG, "Moisture sensor: %d mV (raw: %d)", voltage, adc_raw);
                            
                            // interpret moisture level
                            if (voltage < 1500)
                            {
                                ESP_LOGI(TAG, "Soil status: WET");
                            }
                            else
                            {
                                ESP_LOGI(TAG, "Soil status: DRY");
                            }
                        }
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Moisture sensor not calibrated - skipping");
                    }
                    
                    ESP_LOGI(TAG, "ADC reading complete");
                }
                
                // data request command - confirm state via MQTT
                else if (strcmp(topic, TOPIC_BACKEND_REQUEST_DATA) == 0)
                {
                    ESP_LOGI(TAG, "Data request received - confirming current state");
                    
                    // send s message back  MQTT
                    char confirmation_msg[64];
                    snprintf(confirmation_msg, sizeof(confirmation_msg), 
                             "state_confirmed:auto_%s", auto_care_on ? "on" : "off");
                    publish_mqtt(TOPIC_ESP32_DATA, confirmation_msg);
                    
                    ESP_LOGI(TAG, "State confirmation sent: auto-care is %s", 
                             auto_care_on ? "ENABLED" : "DISABLED");
                }
            }
            #endif


            #ifndef DEBUG_MODE_MAIN
            // Toggle autocare enable command
            if (strcmp(topic, "plant_partner/act_tog_en") == 0)
            {
                auto_care_on = !auto_care_on;

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
            #endif
        }
        // Must be at end of while loop, allows other CPU to activate
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
