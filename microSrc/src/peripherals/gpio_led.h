#ifndef GPIO_LED_H
#define GPIO_LED_H

#include "driver/gpio.h"

#define LED_THRESHOLD 650

// Initialize actuator communication (GPIO 2)
gpio_config_t io_conf_blueLED = {
    .pin_bit_mask = (1ULL << GPIO_NUM_2),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
};

// Initialize actuator communication (GPIO 14)
gpio_config_t io_conf_externalLED = {
    .pin_bit_mask = (1ULL << GPIO_NUM_14),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
};



#endif // GPIO_LED_H