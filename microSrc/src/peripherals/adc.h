#ifndef ADC_LIGHTDIODE_H
#define ADC_LIGHTDIODE_H

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_ATTEN               ADC_ATTEN_DB_12
#define ADC_LIGHT_CHANNEL       ADC_CHANNEL_0
#define ADC_MOISTURE_CHANNEL    ADC_CHANNEL_3
#define LIGHT                   true
#define MOISTURE                false

// ADC Initialization and Configuration Functions
adc_oneshot_unit_handle_t adc_oneshot_unit1_init();
void adc_oneshot_channel_config(bool component, adc_oneshot_unit_handle_t handle);
bool adc_calibration_init(adc_unit_t unit, adc_atten_t atten, adc_cali_handle_t *out_handle);

// ADC Data Reading and Conversion Functions
void adc_read(bool component, adc_oneshot_unit_handle_t handle, int *raw);
void adc_rawToVoltage(adc_cali_handle_t handle, int raw, int *voltage);

#endif // ADC_LIGHTDIODE_H