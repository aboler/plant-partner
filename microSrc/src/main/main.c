// Basic Main Loop

#include <stdio.h>
#include <stdbool.h>
#include "../dataTypes/plantData.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    // Initialize plant structure data with test values
    struct plantData testStructure = {50, 54, 87};

    // Initialize one-shot ADC

    // Initialize actuator communication (GPIO 16) using gpio_config for clarity
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_16),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // Simple observable test: toggle GPIO16 every 500 ms and print state.
    // Use this to verify the pin is being driven (measure with multimeter/logic analyzer).
    bool level = false;
    while (1)
    {
        level = !level;
        gpio_set_level(GPIO_NUM_16, level ? 1 : 0);
        // Print with newline so the monitor shows lines promptly
        printf("GPIO16 set to %d\n", level ? 1 : 0);
        // Delay so toggles are slow and observable (500 ms)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
