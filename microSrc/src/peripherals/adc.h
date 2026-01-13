#ifndef ADC_LIGHTDIODE_H
#define ADC_LIGHTDIODE_H

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_UNIT                ADC_UNIT_1
#define ADC_ATTEN               ADC_ATTEN_DB_12
#define ADC_BITWIDTH            ADC_BITWIDTH_DEFAULT
#define ADC_LIGHT_CHANNEL       ADC_CHANNEL_0
#define ADC_MOISTURE_CHANNEL    ADC_CHANNEL_3
#define LIGHT                   true
#define MOISTURE                false

// ADC Initialization Function
bool adc_init(adc_oneshot_unit_handle_t *unit_handle, bool component, adc_cali_handle_t *calibration_handle);

// ADC Data Reading and Conversion Functions
void adc_read(bool component, adc_oneshot_unit_handle_t handle, int *raw);
void adc_rawToVoltage(adc_cali_handle_t handle, int raw, int *voltage);

#endif // ADC_LIGHTDIODE_H