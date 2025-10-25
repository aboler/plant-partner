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


const static char *TAG = "EXAMPLE";
static int adc_raw[2][10];
static int voltage[2][10];

void app_main(void)
{
    // Initialize plant structure data with test values
    struct plantData testStructure = {0, 0, 0};

    // Initialize one-shot ADC
    //From code
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC1_CHAN0, &config));

    //-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_chan0_handle = NULL;
    bool do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, ADC1_CHAN0, ADC_ATTEN, &adc1_cali_chan0_handle);

    // Configure GPIO16 - io_config defined in gpio_led.h
    gpio_config(&io_conf_blueLED);
    gpio_config(&io_conf_externalLED); // Configure external LED GPIO

    while (1)
    {
        // Read data in from ADC
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN0, &adc_raw[0][0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC1_CHAN0, adc_raw[0][0]);
        if (do_calibration1_chan0) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw[0][0], &voltage[0][0]));
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN0, voltage[0][0]);
        }
        

        // If voltage below threshold, turn LEDs on
        if (voltage[0][0] < LED_THRESHOLD) {
            gpio_set_level(GPIO_NUM_2, 1);  // Internal LED on
            gpio_set_level(GPIO_NUM_14, 1); // External LED on

            // Print LED Status and Voltage level as ESP log
            ESP_LOGI(TAG, "LEDs ON: Voltage %d mV below threshold", voltage[0][0]);
        }
        // If voltage above or equal to threshold, turn LEDs off
        else if (voltage[0][0] >= LED_THRESHOLD)
        {
            gpio_set_level(GPIO_NUM_2, 0);  // Internal LED off
            gpio_set_level(GPIO_NUM_14, 0); // External LED off

            // Print statement to confirm LEDs are off
            ESP_LOGI(TAG, "LEDs OFF: Voltage %d mV above threshold", voltage[0][0]);
        }
        
    }
}
