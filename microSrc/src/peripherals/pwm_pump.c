#include "pwm_pump.h"

void pwm_pump_init(bool component)
{
    // Config HW timer
    ledc_timer_config_t pwm_timer_config = {
        .speed_mode       = PWM_MODE,
        .duty_resolution  = PWM_RESOLUTION,
        .timer_num        = PWM_TIMER,
        .freq_hz          = PWM_FREQUENCY_HZ,  // Set output frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&pwm_timer_config));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t pwm_channel_config;
    if (component == WATER)
    {
        pwm_channel_config = (ledc_channel_config_t){
            .gpio_num       = PWM_WATER_MOTOR_PIN,
            .timer_sel      = PWM_TIMER,
            .speed_mode     = PWM_MODE,
            .channel        = PWM_WATER_CHANNEL,
            .intr_type      = LEDC_INTR_DISABLE,
            .duty           = 0,
            .hpoint         = 0
        };
    }
    else
    {
        pwm_channel_config = (ledc_channel_config_t){
            .gpio_num       = PWM_FERTILIZER_MOTOR_PIN,
            .timer_sel      = PWM_TIMER,
            .speed_mode     = PWM_MODE,
            .channel        = PWM_FERTILIZER_CHANNEL,
            .intr_type      = LEDC_INTR_DISABLE,
            .duty           = 0,
            .hpoint         = 0
        };
    }
    ESP_ERROR_CHECK(ledc_channel_config(&pwm_channel_config));
}

void modify_pump_duty_cycle(bool component, uint8_t duty_cycle)
{
    if (component == WATER)
    {
        ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE, PWM_WATER_CHANNEL, duty_cycle));
        ESP_ERROR_CHECK(ledc_update_duty(PWM_MODE, PWM_WATER_CHANNEL));
    }
    else
    {
        ESP_ERROR_CHECK(ledc_set_duty(PWM_MODE, PWM_FERTILIZER_CHANNEL, duty_cycle));
        ESP_ERROR_CHECK(ledc_update_duty(PWM_MODE, PWM_FERTILIZER_CHANNEL));
    }
}