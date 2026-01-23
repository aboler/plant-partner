#include "heartbeat.h"
#include "gpio.h"
#include "esp_timer.h"

void heartbeat_callback(void *parameters)
{
    toggle_activeHigh_LED(OUTPUT, INTERNAL_BLUE_LED_GPIO);
}

void heartbeat_init()
{
    const esp_timer_create_args_t timer_configuration = {
        .callback = &heartbeat_callback,
        .name = "Heartbeat Interrupt"
    };
    esp_timer_handle_t timer_handle;

    // Initialize LED
    configure_IO(OUTPUT, INTERNAL_BLUE_LED_GPIO);

    // Create timer
    esp_timer_create(&timer_configuration, &timer_handle);
    esp_timer_start_periodic(timer_handle, 500000); // 500ms
}