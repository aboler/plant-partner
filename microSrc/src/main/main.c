// Basic Main Loop

#include <stdio.h>
#include <stdbool.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../dataTypes/plantData.h"
#include "../peripherals/gpio_led.h"
#include "../peripherals/adc_lightDiode.h"


const static char *TAG = "EXAMPLE";
static int adc_raw[2][10];
static int voltage[2][10];

void app_main(void)
{
    struct adcStructure lightDiodeStructure;
    // Initialize plant structure data with test values
    struct plantData testStructure = {50, 54, 87};

    // Initialize one-shot ADC
    //From code
    bool do_calibration1_chan0 = example_adc_calibration_init(ADC_UNIT_1, ADC1_CHAN0, ADC_ATTEN, &lightDiodeStructure);

    // Configure GPIO16 - io_config defined in gpio_led.h
    gpio_config(&io_conf);

    // Simple observable test: toggle GPIO16 every 500 ms and print state.
    // Use this to verify the pin is being driven (measure with multimeter/logic analyzer).
    bool level = false;
    while (1)
    {
        level = !level;
        gpio_set_level(GPIO_NUM_16, level ? 1 : 0);
        // Print with newline so the monitor shows lines promptly
        printf("GPIO16 set to %d\n", level ? 1 : 0);
        // Delay so toggles are slow and observable (500 ms)
        vTaskDelay(pdMS_TO_TICKS(500));
        
        // Read data in from ADC
        ESP_ERROR_CHECK(adc_oneshot_read(lightDiodeStructure.adc1Handle, ADC1_CHAN0, &adc_raw[0][0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC1_CHAN0, adc_raw[0][0]);
        if (do_calibration1_chan0) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(lightDiodeStructure.adc1CaliChan0Handle, adc_raw[0][0], &voltage[0][0]));
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN0, voltage[0][0]);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        // Process data from ADC to get reasonable digital value

        // If value below threshold, turn actuator off (Note: actuator is in form of wired LED, so gpio pin down)

        // If value above threshold, turn actuator on for certain amount of time (define through poll or timer)
    }
}
