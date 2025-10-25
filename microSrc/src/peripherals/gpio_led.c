#include "gpio_led.h"

// Configure an active high LED on the specified GPIO pin
void configure_activeHigh_LED(int gpio_num) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
}

// Set the specified GPIO pin to turn on the active high LED
void set_activeHigh_LED(int gpio_num) {
    ESP_ERROR_CHECK(gpio_set_level(gpio_num, 1));
}

// Clear the specified GPIO pin to turn off the active high LED
void clear_activeHigh_LED(int gpio_num) {
    ESP_ERROR_CHECK(gpio_set_level(gpio_num, 0));
}