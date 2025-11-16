#include "adc.h"

// Initialize ADC Unit 1
adc_oneshot_unit_handle_t adc_oneshot_unit1_init(){
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    return adc1_handle;
}

// Configure ADC Channel 0
void adc_oneshot_channel_config(bool component, adc_oneshot_unit_handle_t handle){
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    
    if (component == LIGHT)
        ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_LIGHT_CHANNEL, &config)); // pin vp
    else // component == MOISTURE
        ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, ADC_MOISTURE_CHANNEL, &config)); // pin vn
}

// Initialize ADC Calibration
bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle)
{
    // Check if configuration is actually valid
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

// Read in ADC Data from specified handle, channel, and store data while error checking
void adc_read(bool component, adc_oneshot_unit_handle_t handle, int *raw)
{
    if (component == LIGHT)
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_LIGHT_CHANNEL, raw));    
    else // component == MOISTURE
        ESP_ERROR_CHECK(adc_oneshot_read(handle, ADC_MOISTURE_CHANNEL, raw)); 
}

// Convert raw ADC data to voltage with calibration handle
void adc_rawToVoltage(adc_cali_handle_t handle, int raw, int *voltage)
{
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(handle, raw, voltage));
}

