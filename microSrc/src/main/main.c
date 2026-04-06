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
#include "../peripherals/uart_driver.h"
#include "../peripherals/communication/ringbuf.h"

#define MAX_TRANSMISSION_ATTEMPTS            5
const static char *TAG = "DEBUG";

const static char *TOPIC_AUTO_NOTIF = "plant_partner/auto_notif";
const static char *TOPIC_ACT_COMPLETE = "plant_partner/act_compl";
const static char *TOPIC_INIT_TOGGLE = "plant_partner/autocare_startup";
const static char *TOPIC_CHECK_TOGGLE = "plant_partner/act_tog_en";
const static char *TOPIC_CHECK_SENSORS = "plant_partner/ack";
const static char *TOPIC_TASK_MENU    = "plant_partner/task";
const static char *TOPIC_START_UP = "plant_partner/esp_startup";
const static char *ACTIVATION_MESSAGE_AUTOCARE = "Autocare enabled";
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

    // Initialize UART for RS485 communication with nutrient sensor
    uart_rs485_init();

    publish_mqtt(TOPIC_START_UP, "Start up");

    while (1)
    {
        if (mqtt_check_buffer_ready())
        {
            // Recieve topic and message
            message = read_data();
            topic = read_topic();

            ESP_LOGI("While Loop, Most Recently read ", "Topic: %s, Data: %s", topic, message);

            // Toggle autocare on/off via startup topic
            if (strcmp(topic, TOPIC_INIT_TOGGLE) == 0)
            {
                if (strcmp(message, "false") == 0)
                    auto_care_on = false;
                else if (strcmp(message, "true") == 0)
                    auto_care_on = true;
                else
                    auto_care_on = !auto_care_on;

                ESP_LOGI(TAG, "Toggle autocare to: %d", auto_care_on);
            }
            // Toggle autocare enable command
            else if (strcmp(topic, TOPIC_CHECK_TOGGLE) == 0)
            {
                if (strcmp(message, ACTIVATION_MESSAGE_AUTOCARE) == 0)
                {
                    auto_care_on = true;
                    publish_mqtt(TOPIC_AUTO_NOTIF, MESSAGE_AUTOCARE_ON);
                }
                else
                {
                    auto_care_on = false;
                    publish_mqtt(TOPIC_AUTO_NOTIF, MESSAGE_AUTOCARE_OFF);
                }
                ESP_LOGI(TAG, "Toggle autocare to: %d", auto_care_on);
            }
            // Read all sensors and send to database
            else if (strcmp(topic, TOPIC_CHECK_SENSORS) == 0)
            {
                publish_mqtt("plant_partner/state", "ENTERED CHECK SENSORS");
                ESP_LOGI(TAG, "Entered Check Sensors");

                // Read moisture
                if (moisture_calibration_successful)
                {
                    adc_read(MOISTURE, adc1_handle, &adc_raw);
                    adc_rawToVoltage(moisture_cali_adc1_handle, adc_raw, &voltage);

                    if (voltage < 0)
                        ESP_LOGW(TAG, "Invalid moisture reading");
                    else
                        p_ptr->soilMoisture = voltage;
                }

                // Read light
                if (light_calibration_successful)
                {
                    adc_read(LIGHT, adc1_handle, &adc_raw);
                    adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);

                    if (voltage < 0)
                        ESP_LOGW(TAG, "Invalid light reading");
                    else
                        p_ptr->lightIntensity = voltage;
                }

                // Read nutrients via RS485
                ESP_LOGI(TAG, "Reading from RS485-connected nutrient sensor...");
                uart_rs485_read(p_ptr);
                ESP_LOGI(TAG, "Finished RS485 read...");

                ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d], N[%d], P[%d], K[%d]",
                         p_ptr->lightIntensity, p_ptr->soilMoisture,
                         p_ptr->nLevel, p_ptr->pLevel, p_ptr->kLevel);

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

                publish_mqtt(TOPIC_ACT_COMPLETE, MESSAGE_WATER_DONE);
            }
            // Task menu placeholder
            else if (strcmp(topic, TOPIC_TASK_MENU) == 0)
            {
                // TODO: handle task menu messages
            }
            else
            {
                ESP_LOGI(TAG, "ERROR, TOPIC: %s IS NOT RECOGNIZED", topic);
            }
        }

        // Must be at end of while loop, allows other CPU to activate
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
