// Basic Main Loop

#include <stdio.h>
#include "../dataTypes/plantData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

void app_main(void)
{
    // Initialize plant structure data with test values
    struct plantData testStructure = {16, 54, 87};

    // Initialize one-shot ADC

    // Initialize actuator communication

    while(1)
    {
        printf("Hello World");
        // Read data in from ADC
        
        // Process data from ADC to get reasonable digital value

        // If value below threshold, turn actuator off (Note: actuator is in form of wired LED, so gpio pin down)

        // If value above threshold, turn actuator on for certain amount of time (define through poll or timer)
    }
}
