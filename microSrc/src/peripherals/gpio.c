#include "gpio.h"

// Declarations
static bool external_LED_state;
static bool internal_blue_LED_state; 
void configure_IO(bool ioType, uint8_t gpio_num);
void set_activeHigh_LED(bool ioType, uint8_t gpio_num);
void clear_activeHigh_LED(bool ioType, uint8_t gpio_num);
void toggle_activeHigh_LED(bool ioType, uint8_t gpio_num);

// Configure an active high LED or pull up switch on the specified GPIO pin
void configure_IO(bool ioType, uint8_t gpio_num) {
    if(ioType == OUTPUT)
    {
        if(gpio_num == EXTERNAL_LED_GPIO)
            external_LED_state = false;
        else if(gpio_num == INTERNAL_BLUE_LED_GPIO)
            internal_blue_LED_state = false;

        gpio_config_t configuration = {
            .pin_bit_mask = (1ULL << gpio_num),
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };

        ESP_ERROR_CHECK(gpio_config(&configuration));
    }
    else // ioType == INPUT
    {
        gpio_config_t configuration = {
            .pin_bit_mask = (1ULL << gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };

        ESP_ERROR_CHECK(gpio_config(&configuration));

    }
}

// Set the specified GPIO pin to turn on the active high LED
void set_activeHigh_LED(bool ioType, uint8_t gpio_num) {
    if(ioType == OUTPUT)
    {
        ESP_ERROR_CHECK(gpio_set_level(gpio_num, 1));

        if(gpio_num == EXTERNAL_LED_GPIO)
            external_LED_state = true;
        else if(gpio_num == INTERNAL_BLUE_LED_GPIO)
            internal_blue_LED_state = true;
    }
}

// Clear the specified GPIO pin to turn off the active high LED
void clear_activeHigh_LED(bool ioType, uint8_t gpio_num) {
    if(ioType == OUTPUT)
    {
        ESP_ERROR_CHECK(gpio_set_level(gpio_num, 0));

        if(gpio_num == EXTERNAL_LED_GPIO)
            external_LED_state = false;
        else if(gpio_num == INTERNAL_BLUE_LED_GPIO)
            internal_blue_LED_state = false;
    }
}

void toggle_activeHigh_LED(bool ioType, uint8_t gpio_num) {
    if(ioType == OUTPUT)
    {
        if(gpio_num == EXTERNAL_LED_GPIO)
        {
            external_LED_state = !external_LED_state;
            ESP_ERROR_CHECK(gpio_set_level(gpio_num, external_LED_state));

        }
        else if(gpio_num == INTERNAL_BLUE_LED_GPIO)
        {
            internal_blue_LED_state = !internal_blue_LED_state;
            ESP_ERROR_CHECK(gpio_set_level(gpio_num, internal_blue_LED_state));
        }
    }
}