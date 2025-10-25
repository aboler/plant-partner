// Basic Main Loop

#include <stdio.h>
#include <stdbool.h>
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../dataTypes/plantData.h"
#include "../peripherals/gpio_led.h"
#include "../peripherals/adc_lightDiode.h"


const static char *TAG = "EXAMPLE";
static int adc_raw[2][10];
static int voltage[2][10];

static bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    if (!calibrated) {
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = atten,
            .bitwidth = ADC_BITWIDTH_DEFAULT,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }

    *out_handle = handle;

    return calibrated;
}

void app_main(void)
{
    // Initialize plant structure data with test values
    struct plantData testStructure = {50, 54, 87};

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
        
        printf("Hello World");
        // Read data in from ADC
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC1_CHAN0, &adc_raw[0][0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC1_CHAN0, adc_raw[0][0]);
        if (do_calibration1_chan0) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw[0][0], &voltage[0][0]));
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC1_CHAN0, voltage[0][0]);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        // Process data from ADC to get reasonable digital value

        // If value below threshold, turn actuator off (Note: actuator is in form of wired LED, so gpio pin down)

        // If value above threshold, turn actuator on for certain amount of time (define through poll or timer)
    }
}
