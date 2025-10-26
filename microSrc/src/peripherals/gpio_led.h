#ifndef GPIO_LED_H
#define GPIO_LED_H

#include "driver/gpio.h"

#define LED_THRESHOLD 650
#define EXTERNAL_LED_GPIO GPIO_NUM_14
#define INTERNAL_BLUE_LED_GPIO GPIO_NUM_2

// GPIO Configuration & Control for active High LEDs
void configure_activeHigh_LED(int gpio_num);
void set_activeHigh_LED(int gpio_num);
void clear_activeHigh_LED(int gpio_num);

#endif // GPIO_LED_H