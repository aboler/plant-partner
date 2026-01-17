#include "adc.h"

static bool adc1_initialized = false;

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

// Initialize ADC calibration for converting raw ADC data to voltage
static bool adc_calibration_init(adc_cali_handle_t *calibration_handle)
{
    // Confirm if configuration valid
    esp_err_t ret = ESP_FAIL;
    bool calibrated = false;

    if (!calibrated) {
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = ADC_UNIT,
            .atten = ADC_ATTEN,
            .bitwidth = ADC_BITWIDTH,
        };
        ret = adc_cali_create_scheme_line_fitting(&cali_config, calibration_handle);
        if (ret == ESP_OK) {
            calibrated = true;
        }
    }

    return calibrated;
}

// Configure ADC Channel
static void adc_configure_component_channel(bool component, adc_oneshot_unit_handle_t *adc1_unit_handle){
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BITWIDTH,
    };
    
    if (component == LIGHT)
        ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc1_unit_handle, ADC_LIGHT_CHANNEL, &config)); // pin vp
    else // component == MOISTURE
        ESP_ERROR_CHECK(adc_oneshot_config_channel(*adc1_unit_handle, ADC_MOISTURE_CHANNEL, &config)); // pin vn
}

// Initialize ADC1 unit
static void adc1_unit_init(adc_oneshot_unit_handle_t *adc1_unit_handle){
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, adc1_unit_handle));
}

bool adc_init(adc_oneshot_unit_handle_t *unit_handle, bool component, adc_cali_handle_t *calibration_handle)
{
    // Initialize ADC1 if hasn't been done already
    if (!adc1_initialized)
    {
        adc1_unit_init(unit_handle);

        adc1_initialized = true;
    }

    // Configure channel for specific component
    adc_configure_component_channel(component, unit_handle);

    return adc_calibration_init(calibration_handle);
}
