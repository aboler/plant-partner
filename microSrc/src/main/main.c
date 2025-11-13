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
#include "../peripherals/gpio_led.h"
#include "../peripherals/adc.h"
#include "../peripherals/pwm_pump.h"

void app_main(void)
{
    // Debug Tag
    const static char *TAG = "DEBUG";

    // Initialize plant structure data with test values
    struct plantData plant_data = {0, 0, 0};

    // Declare variables for ADC raw data and voltage
    int adc_raw;
    int voltage;
    bool switch0, switch1, switch2;
    switch0 = switch1 = switch2 = false;

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

    // Configure LEDs
    // initialize_interrupts();
    configure_IO(OUTPUT, INTERNAL_BLUE_LED_GPIO);
    configure_IO(OUTPUT, EXTERNAL_LED_GPIO);
    configure_IO(INPUT, SWITCH0_GPIO);
    configure_IO(INPUT, SWITCH1_GPIO);
    configure_IO(INPUT, SWITCH2_GPIO);

    // Configure PWM and set to 30/256
    //pwm_pump_init();
    //modify_pump_duty_cycle(30);

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
                    set_activeHigh_LED(OUTPUT, INTERNAL_BLUE_LED_GPIO);
                    set_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Store data into plant structure
                    plant_data.lightData = voltage;

                    // Print LED Status and Voltage level as ESP log
                    ESP_LOGI(TAG, "LEDs ON: Voltage %d mV is below threshold", plant_data.lightData);
                }
                // If voltage above or equal to threshold, turn LEDs off
                else if (voltage >= LED_THRESHOLD)
                {
                    // Turn off Internal and External LEDs
                    clear_activeHigh_LED(OUTPUT, INTERNAL_BLUE_LED_GPIO);
                    clear_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

                    // Store data into plant structure
                    plant_data.lightData = voltage;

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
                voltage = 100 * (2600 - adc_raw)/2600;
                //adc_rawToVoltage(light_cali_adc1_handle, adc_raw, &voltage);
                if(voltage < 65)
                    ESP_LOGI(TAG, "DRY: ADC%d Channel[%d] Showing How Wet: %d percent", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);
                else
                    ESP_LOGI(TAG, "WET: ADC%d Channel[%d] Showing How Wet: %d percent", ADC_UNIT_1 + 1, ADC_MOISTURE_CHANNEL, voltage);
            
                // Update value
                plant_data.waterData = voltage;
            }
            
            switch1 = currentSwitchLevel;
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        // currentSwitchLevel = (bool)gpio_get_level(SWITCH2_GPIO);
        // if(currentSwitchLevel != switch2)
        // {
        //     toggle_activeHigh_LED(OUTPUT, INTERNAL_BLUE_LED_GPIO);
        //     toggle_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);
        //     switch2 = currentSwitchLevel;
        //     vTaskDelay(pdMS_TO_TICKS(200));
        // }

        
        //vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
