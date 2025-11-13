#include "gpio.h"
#include "esp_log.h"
#include "esp_mac.h"
#include <inttypes.h>

// Current Issues: I can get the interrupts to work if they both use the same ISR
// But like no, and if I create three ISR's it causes a memory issue and idk how fix

// Declarations
static bool external_LED_state;
static bool internal_blue_LED_state; 
static void __attribute__((section(".iram1.0"))) switch0_isr_handler(void *arg);
void configure_IO(bool ioType, uint8_t gpio_num);
void set_activeHigh_LED(bool ioType, uint8_t gpio_num);
void clear_activeHigh_LED(bool ioType, uint8_t gpio_num);
void toggle_activeHigh_LED(bool ioType, uint8_t gpio_num);

// ISR routine for switches
static void __attribute__((section(".iram1.0"))) switch0_isr_handler(void *arg)
{
    uint8_t *pin_address = (uint8_t *)arg;
    toggle_activeHigh_LED(OUTPUT, INTERNAL_BLUE_LED_GPIO);
    toggle_activeHigh_LED(OUTPUT, EXTERNAL_LED_GPIO);

    gpio_isr_handler_add(*pin_address, switch0_isr_handler, pin_address);
    gpio_intr_enable(*pin_address);
}

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
            .intr_type = GPIO_INTR_POSEDGE,
        };

        ESP_ERROR_CHECK(gpio_config(&configuration));

        //gpio_set_intr_type(gpio_num, GPIO_INTR_ANYEDGE);
        //gpio_isr_handler_add(gpio_num, switch0_isr_handler, &gpio_num);
        //gpio_intr_enable(gpio_num);

    }
}

void initialize_interrupts()
{
    gpio_install_isr_service(0);
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