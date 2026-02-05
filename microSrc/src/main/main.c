// Standard C Libraries
#include <stdio.h>
#include <stdbool.h>

// ESP-IDF Libraries
#include "esp_log.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Data Types
#include "../dataTypes/plantData.h"

// Peripherals
#include "../peripherals/adc.h"
#include "../peripherals/gpio.h"
#include "../peripherals/heartbeat.h"
#include "../peripherals/pwm_pump.h"
#include "../communication_protocols/http.h"
#include "../wifi/wifi.h"
#include "../communication_protocols/mqtt_proto.h"

//static struct plantData pv1 = {"Sunflower",1,2,3,4,5};

//this configuration for testing purposes
#define MQTT_ON
#ifdef MQTT_ON
#define msgSize 256
#define topicName 64
char messageBuffer[msgSize];
char topic[topicName];
#endif

static esp_err_t nvs_init(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    return ret;
}

const static char *TAG = "DEBUG";


// Uncommenting RTOS stuff will produce test updates

/*void http_task(void *pv) {
    
    struct plantData *plant = (struct plantData *) pv;
    esp_http_client_handle_t client = http_configure_handle();
    http_put_plant_data(client,plant);
    while (1) {
        ESP_LOGI(TAG, "HTTP request...");
        esp_err_t err = esp_http_client_perform(client);
        ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}*/

// all Networking depends on nvs_init()

void app_main(void)
{  
    //Used by all
    ESP_ERROR_CHECK(nvs_init());
    

    start_wifi();
    

    //MQTT Style
    #ifdef MQTT_ON
    mqtt_connect();
    //wait until ack knowledge

    while(!mqtt_poll_from(messageBuffer,msgSize,"plant_partner/ack"))
    mqtt_publish("plant_partner/ack","ack",0);
    #else
    
    esp_http_client_handle_t client;
    client = http_configure_handle();
    http_put_plant_data(client,p_ptr);

    #endif 
    // Declare variables
    int adc_raw, voltage;
    bool auto_care_on, light_calibration_successful, moisture_calibration_successful, current_switch_level, switch0;
    auto_care_on = true; // TBD: CHANGE THIS ONCE DATABASE SIGNAL CAN BE RECEIVED AND CAN SIGNAL TOGGLING ON AND OFF AUTO CARE

    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t light_cali_adc1_handle, moisture_cali_adc1_handle;
    light_cali_adc1_handle = moisture_cali_adc1_handle = NULL;

    struct plantData p = {"Sunflower", 1, 2, 3, 4, 5};
    struct plantData* p_ptr = &p;




    // TO:DO mutex stuff maybe
    //xTaskCreate(http_task, "http_task", 8192, &pv1, 5, NULL);


    // Initialize ADC for photoresistor and moisture sensor
    light_calibration_successful = adc_init(&adc1_handle, LIGHT, &light_cali_adc1_handle);
    moisture_calibration_successful = adc_init(&adc1_handle, MOISTURE, &moisture_cali_adc1_handle);

    // Configure LEDs and inputs
    heartbeat_init();
    configure_IO(OUTPUT, EXTERNAL_LED_GPIO);
    configure_IO(INPUT, SWITCH0_GPIO);

    clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);
    switch0 = (bool)gpio_get_level(SWITCH0_GPIO);

    // Configure PWMs
    pwm_pump_init(WATER);
    pwm_pump_init(FERTLIZER);

    while (1)
    {
        // STAND IN: Represents auto. scheduling signal being received
        current_switch_level = (bool)gpio_get_level(SWITCH0_GPIO);
        
        if(current_switch_level != switch0)
        {
            // 1. Assess and store light if configured
            if(light_calibration_successful)
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
            if(moisture_calibration_successful)
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
                if(p_ptr->soilMoisture < 100)
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


            //HTTP Protocol
            #ifndef MQTT_ON
            // 5. Send data to database
            http_put_plant_data(client,p_ptr);
            ESP_LOGI(TAG, "HTTP request...");
            esp_err_t err = esp_http_client_perform(client);
            ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));
            ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", p_ptr->lightIntensity, p_ptr->soilMoisture);
            #endif
            // 6. Update switch value
            switch0 = current_switch_level;
        }  

        // Must be at end of while loop, allows other CPU to activate
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
