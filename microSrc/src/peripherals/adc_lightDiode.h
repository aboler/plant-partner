#ifndef ADC_LIGHTDIODE_H
#define ADC_LIGHTDIODE_H

#include "adcStructure.h"

#define ADC_ATTEN    ADC_ATTEN_DB_12
#define ADC1_CHAN0   ADC_CHANNEL_0

bool example_adc_calibration_init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten, struct adcStructure *lightDiodeStructure);

#endif