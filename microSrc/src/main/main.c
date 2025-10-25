// Basic Main Loop

#include <stdio.h>
#include "../dataTypes/plantData.h"

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
