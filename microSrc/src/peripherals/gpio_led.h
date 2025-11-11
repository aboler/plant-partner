#ifndef GPIO_LED_H
#define GPIO_LED_H

#include "driver/gpio.h"

#define LED_THRESHOLD          650
#define EXTERNAL_LED_GPIO      GPIO_NUM_14
#define INTERNAL_BLUE_LED_GPIO GPIO_NUM_2
#define SWITCH0_GPIO           GPIO_NUM_21
#define SWITCH1_GPIO           GPIO_NUM_19
#define SWITCH2_GPIO           GPIO_NUM_18
#define OUTPUT                 true
#define INPUT                  false

// GPIO Configuration & Control for active High LEDs
void configure_IO(bool ioType, uint8_t gpio_num);
void initialize_Interrupts();
void set_activeHigh_LED(bool ioType, uint8_t gpio_num);
void clear_activeHigh_LED(bool ioType, uint8_t gpio_num);
void toggle_activeHigh_LED(bool ioType, uint8_t gpio_num);

#endif // GPIO_LED_H