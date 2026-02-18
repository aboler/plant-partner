#ifndef GPIO_LED_H
#define GPIO_LED_H

#include "driver/gpio.h"

#define LED_THRESHOLD          1250  // Voltage threshold in mV for LED control
#define EXTERNAL_LED_GPIO      GPIO_NUM_4
#define INTERNAL_BLUE_LED_GPIO GPIO_NUM_2
#define SWITCH0_GPIO           GPIO_NUM_35
#define SWITCH1_GPIO           GPIO_NUM_32
#define SWITCH2_GPIO           GPIO_NUM_33
#define SWITCH3_GPIO           GPIO_NUM_25
#define SWITCH4_GPIO           GPIO_NUM_26
#define OUTPUT                 true
#define INPUT                  false

// GPIO Configuration & Control for active High LEDs
void configure_IO(bool ioType, uint8_t gpio_num);
void initialize_interrupts();
void set_activeHigh_LED(bool ioType, uint8_t gpio_num);
void clear_activeHigh_LED(bool ioType, uint8_t gpio_num);
void toggle_activeHigh_LED(bool ioType, uint8_t gpio_num);

#endif // GPIO_LED_H