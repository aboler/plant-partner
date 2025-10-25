#ifndef ADC_LIGHTDIODE_H
#define ADC_LIGHTDIODE_H

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_ATTEN    ADC_ATTEN_DB_12
#define ADC1_CHAN0   ADC_CHANNEL_0

bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, adc_cali_handle_t *out_handle);

#endif