#ifndef GPIO_LED_H
#define GPIO_LED_H

#include "driver/gpio.h"

// Initialize actuator communication (GPIO 16)
gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << GPIO_NUM_16),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
};

#endif // GPIO_LED_H