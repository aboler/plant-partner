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
#include "../peripherals/gpio.h"
#include "../peripherals/adc.h"
#include "../peripherals/pwm_pump.h"
#include "../wifi/wifi.h"
#include "../http/http.h"


static struct plantDataUpdate pv1 = {"Sunflower",1,2,3,4,5};


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
    
    struct plantDataUpdate *plant = (struct plantDataUpdate *) pv;
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

   
    // Debug Tag
    
    ESP_ERROR_CHECK(nvs_init());
    // Initialize plant structure data with test values

    start_wifi();
    struct plantData plant_data = {0, 0, 0, 0, 0};
    //redundant for testing/prototype

    //Instantiate handle for sending put requests.
    struct plantDataUpdate p = {"Sunflower",1,2,3,4,5};
    //For inputting data
    struct plantDataUpdate* p_ptr = &p;

    esp_http_client_handle_t client = http_configure_handle();
    http_put_plant_data(client,p_ptr);


    //TO:DO mutex stuff maybe
    //xTaskCreate(http_task, "http_task", 8192, &pv1, 5, NULL);
  
    // Declare variables
    int adc_raw;
    int voltage;
    bool switch0, switch1, switch2, switch3;
    switch0 = switch1 = switch2 = switch3 = false;

    // Initialize ADC for photoresistor
    adc_oneshot_unit_handle_t adc1_handle = adc_oneshot_unit1_init();
    adc_oneshot_channel_config(LIGHT, adc1_handle);
    adc_cali_handle_t light_cali_adc1_handle = NULL;
    bool light_calibration_successful = adc_calibration_init(ADC_UNIT_1, ADC_ATTEN, &light_cali_adc1_handle);

    // Initialize ADC for moisture sensor
    //adc_oneshot_unit_handle_t adc1_handle = adc_oneshot_unit1_init();
    adc_oneshot_channel_config(MOISTURE, adc1_handle);
    adc_cali_handle_t moisture_cali_adc1_handle = NULL;
    bool moisture_calibration_successful = adc_calibration_init(ADC_UNIT_1, ADC_ATTEN, &moisture_cali_adc1_handle);

    // Configure LEDs and inputs
    // initialize_interrupts(); // Put if want input interrupts but you'll have ot uncomment stuff in gpio
    configure_IO(OUTPUT, INTERNAL_BLUE_LED_GPIO);
    configure_IO(OUTPUT, EXTERNAL_LED_GPIO);
    configure_IO(INPUT, SWITCH0_GPIO);
    configure_IO(INPUT, SWITCH1_GPIO);
    configure_IO(INPUT, SWITCH2_GPIO);
    configure_IO(INPUT, SWITCH3_GPIO);

    // Configure PWM
    pwm_pump_init();

    while (1)
    {
        bool currentSwitchLevel = (bool)gpio_get_level(SWITCH0_GPIO);
        // ESP_LOGI(TAG, "Level: %d", currentSwitchLevel);
        // vTaskDelay(pdMS_TO_TICKS(1000));
        
        if(currentSwitchLevel != switch0)
        {
            // Update light value and determine if it's dark enough to turn on an LED

            // Read raw data in from ADC
            adc_read(LIGHT, adc1_handle, &adc_raw);
            //ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_LIGHT_CHANNEL, adc_raw);

            // If calibration is enabled, convert raw data to voltage
            if (light_calibration_successful) 
            {
                adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);
                ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC_LIGHT_CHANNEL, voltage);
            
                // Error handling for voltage reading
                if (voltage < 0) 
                {
                    ESP_LOGW(TAG, "Invalid voltage reading: %d mV", voltage);
                }
                // If voltage below threshold, turn LEDs on
                else if (voltage < LED_THRESHOLD) 
                {
                    // Turn on Internal and External LEDs
                    set_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Store data into plant structure
                    plant_data.lightData = voltage;
                    //Temporary redundant stor
                    p_ptr->lightIntensity = (float)voltage;
                    // Print LED Status and Voltage level as ESP log
                    ESP_LOGI(TAG, "LEDs ON: Voltage %d mV is below threshold", plant_data.lightData);
                }
                // If voltage above or equal to threshold, turn LEDs off
                else if (voltage >= LED_THRESHOLD)
                {
                    // Turn off Internal and External LEDs
                    clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Store data into plant structure
                    plant_data.lightData = voltage;
                    
                    //Temporary Redundant Storage for Plant Put test
                    p_ptr->lightIntensity = (float)voltage;

                    // Print statement to confirm LEDs are off
                    ESP_LOGI(TAG, "LEDs OFF: Voltage %d mV is above threshold", plant_data.lightData);
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
                plant_data.waterData = voltage;
                //Temp redundant update 
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

            ESP_LOGI(TAG, "Plant data: Light[%d], Moisture:[%d]", plant_data.lightData, plant_data.waterData);

            switch3 = currentSwitchLevel;
        }

            toggle_activeHigh_LED(OUTPUT, INTERNAL_BLUE_LED_GPIO);
            vTaskDelay(pdMS_TO_TICKS(700));   
    }

    

}
