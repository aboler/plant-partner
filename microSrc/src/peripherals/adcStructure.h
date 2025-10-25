#ifndef ADCSTRUCTURE_H
#define ADCSTRUCTURE_H

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

struct adcStructure
{
    adc_oneshot_unit_handle_t adc1Handle;
    adc_oneshot_unit_init_cfg_t initConfig;
    adc_oneshot_chan_cfg_t config;
    adc_cali_handle_t adc1CaliChan0Handle;
};

#endif