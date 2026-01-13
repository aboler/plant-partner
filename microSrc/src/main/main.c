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
#include "../http/http.h"
#include "../wifi/wifi.h"

//static struct plantData pv1 = {"Sunflower",1,2,3,4,5};

static esp_err_t nvs_init(){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    return ret;
}

const static char *TAG = "DEBUG";


//Uncommenting RTOS stuff will produce test updates

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

//all Networking depends on nvs_init()

void app_main(void)
{
    // Declare variables
    int adc_raw, voltage;
    bool currentSwitchLevel, switch0, switch1, switch2, switch3;
    switch0 = switch1 = switch2 = switch3 = false;

    adc_oneshot_unit_handle_t adc1_handle;
    adc_cali_handle_t light_cali_adc1_handle, moisture_cali_adc1_handle;
    light_cali_adc1_handle = moisture_cali_adc1_handle = NULL;
    bool light_calibration_successful, moisture_calibration_successful;

    struct plantData p = {"Sunflower", 1, 2, 3, 4, 5};
    struct plantData* p_ptr = &p;

    esp_http_client_handle_t client;
   
    // Debug Tag
    ESP_ERROR_CHECK(nvs_init());

    start_wifi();

    client = http_configure_handle();
    http_put_plant_data(client,p_ptr);


    //TO:DO mutex stuff maybe
    //xTaskCreate(http_task, "http_task", 8192, &pv1, 5, NULL);


    // Initialize ADC for photoresistor and moisture sensor
    light_calibration_successful = adc_init(&adc1_handle, LIGHT, &light_cali_adc1_handle);
    moisture_calibration_successful = adc_init(&adc1_handle, MOISTURE, &moisture_cali_adc1_handle);

    // Configure LEDs and inputs
    heartbeat_init();
    configure_IO(OUTPUT, EXTERNAL_LED_GPIO);
    configure_IO(INPUT, SWITCH0_GPIO);
    configure_IO(INPUT, SWITCH1_GPIO);
    configure_IO(INPUT, SWITCH2_GPIO);
    configure_IO(INPUT, SWITCH3_GPIO);

    // Configure PWM
    pwm_pump_init();

    while (1)
    {
        currentSwitchLevel = (bool)gpio_get_level(SWITCH0_GPIO);
        
        if(currentSwitchLevel != switch0)
        {
            // Update light value and determine if it's dark enough to turn on an LED

            // Read raw data in from ADC
            adc_read(LIGHT, adc1_handle, &adc_raw);

            // If calibration is enabled, convert raw data to voltage
            if (light_calibration_successful) 
            {
                adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);
            
                // Error handling for voltage reading
                if (voltage < 0) 
                {
                    ESP_LOGW(TAG, "Invalid voltage reading: %d mV", voltage);
                }
                // If voltage below threshold, turn LED on
                else if (voltage < LED_THRESHOLD) 
                {
                    // Turn on External LED
                    set_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Store data into plant structure
                    p_ptr->lightIntensity = (float)voltage;

                    // Print LED Status and Voltage level as ESP log
                    ESP_LOGI(TAG, "LED ON: Voltage %d mV is below threshold", p_ptr->lightIntensity);
                }
                // If voltage above or equal to threshold, turn LEDs off
                else if (voltage >= LED_THRESHOLD)
                {
                    // Turn off External LED
                    clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Store data into plant structure
                    p_ptr->lightIntensity = (float)voltage;

                    // Print statement to confirm LED is off
                    ESP_LOGI(TAG, "LED OFF: Voltage %d mV is above threshold", p_ptr->lightIntensity);
                }
            }

            // Update switch value
            switch0 = currentSwitchLevel;
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        currentSwitchLevel = (bool)gpio_get_level(SWITCH1_GPIO);
        if(currentSwitchLevel != switch1)
        {
            // Update moisture value

            // Read raw data in from ADC
            adc_read(MOISTURE, adc1_handle, &adc_raw);

            // If calibration is enabled, convert raw data to a moisture value
            if (moisture_calibration_successful) 
            {
                voltage = adc_raw;
                //adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);
                if(voltage < 65)
                    ESP_LOGI(TAG, "DRY: ADC%d Channel[%d] Showing How Wet: %d ", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);
                else
                    ESP_LOGI(TAG, "WET: ADC%d Channel[%d] Showing How Wet: %d ", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);
            
                // Update value
                p_ptr->soilMoisture = voltage;
            }
            
            switch1 = currentSwitchLevel;
        }
        currentSwitchLevel = (bool)gpio_get_level(SWITCH2_GPIO);
        if(currentSwitchLevel != switch2)
        {
            // Toggle power of water motor

            if (currentSwitchLevel)
                modify_pump_duty_cycle(PWM_DUTY_100_PERCENT);
            else
                modify_pump_duty_cycle(0);

            switch2 = currentSwitchLevel;
        }
        currentSwitchLevel = (bool)gpio_get_level(SWITCH3_GPIO);
        if(currentSwitchLevel != switch3)
        {
            // TBD: Send wifi post
            
            http_put_plant_data(client,p_ptr);
            ESP_LOGI(TAG, "HTTP request...");
            esp_err_t err = esp_http_client_perform(client);
            ESP_LOGI(TAG, "HTTP done: %s", esp_err_to_name(err));

            ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", p_ptr->lightIntensity, p_ptr->soilMoisture);

            switch3 = currentSwitchLevel;
        }
            vTaskDelay(pdMS_TO_TICKS(700));   
    }

    

}
