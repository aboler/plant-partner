#ifndef ADC_LIGHTDIODE_H
#define ADC_LIGHTDIODE_H

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_ATTEN    ADC_ATTEN_DB_12

// ADC Initialization and Configuration Functions
adc_oneshot_unit_handle_t adc_oneshot_unit1_init();
void adc_oneshot_channel_config(adc_oneshot_unit_handle_t handle);
bool adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);

// ADC Data Reading and Conversion Functions
void adc_read(adc_oneshot_unit_handle_t handle, adc_channel_t channel, int *raw);
void adc_rawToVoltage(adc_cali_handle_t handle, int raw, int *voltage);

#endif // ADC_LIGHTDIODE_H