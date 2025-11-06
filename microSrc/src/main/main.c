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
#include "../peripherals/adc_lightDiode.h"
#include "../peripherals/pwm_pump.h"

void app_main(void)
{
    // Debug Tag
    const static char *TAG = "DEBUG";

    // Initialize plant structure data with test values
    struct plantData plant_data = {0, 0, 0};

    // Declare arrays for ADC raw data and voltage
    int adc_raw;
    int voltage;

    // Initialize ADC Unit 1 Channel 0
    adc_oneshot_unit_handle_t adc1_handle = adc_oneshot_unit1_init();

    // Configure ADC Channel 0
    adc_oneshot_channel_config(adc1_handle);

    // ADC1 CH0 Calibration Init
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    bool do_calibration1_chan0 = adc_calibration_init(ADC_UNIT_1, ADC1_CHAN0, ADC_ATTEN, &adc1_cali_chan0_handle);

    // Configure LEDs
    configure_activeHigh_LED(INTERNAL_BLUE_LED_GPIO);
    configure_activeHigh_LED(EXTERNAL_LED_GPIO);

    // Configure PWM and set to 30/256
    pwm_pump_init();
    modify_pump_duty_cycle(30);

    while (1)
    {
        // Read raw data in from ADC
        adc_read(adc1_handle, ADC1_CHAN0, &adc_raw);
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC1_CHAN0, adc_raw);

        // If calibration is enabled, convert raw data to voltage
        if (do_calibration1_chan0) {
            adc_rawToVoltage(adc1_cali_chan0_handle, adc_raw, &voltage);
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN0, voltage);
        }
        
        // Error handling for voltage reading
        if (voltage < 0) {
            ESP_LOGW(TAG, "Invalid voltage reading: %d mV", voltage);
        }
        // If voltage below threshold, turn LEDs on
        else if (voltage < LED_THRESHOLD) {
            // Turn on Internal and External LEDs
            set_activeHigh_LED(INTERNAL_BLUE_LED_GPIO);
            set_activeHigh_LED(EXTERNAL_LED_GPIO);

            // Store data into plant structure
            plant_data.lightData = voltage;

            // Print LED Status and Voltage level as ESP log
            ESP_LOGI(TAG, "LEDs ON: Voltage %d mV below threshold", voltage);
        }
        // If voltage above or equal to threshold, turn LEDs off
        else if (voltage >= LED_THRESHOLD){
            // Turn off Internal and External LEDs
            clear_activeHigh_LED(INTERNAL_BLUE_LED_GPIO);
            clear_activeHigh_LED(EXTERNAL_LED_GPIO);

            // Store data into plant structure
            plant_data.lightData = voltage;

            // Print statement to confirm LEDs are off
            ESP_LOGI(TAG, "LEDs OFF: Voltage %d mV above threshold", voltage);
        }
        
    }
}
