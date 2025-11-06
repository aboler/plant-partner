#ifndef PWM_PUMP_H
#define PWM_PUMP_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define PWM_MOTOR_PIN         GPIO_NUM_12
#define PWM_TIMER             LEDC_TIMER_0
#define PWM_MODE              LEDC_LOW_SPEED_MODE
#define PWM_CHANNEL           LEDC_CHANNEL_0
#define PWM_RESOLUTION        LEDC_TIMER_8_BIT
#define PWM_FREQUENCY_HZ      4000
#define PWM_DUTY_100_PERCENT  256 // 2^8
#define PWM_DUTY_50_PERCENT   128
#define PWM_DUTY_0_PERCENT    0


// PWM Initialization and Configuration Functions
void pwm_pump_init();
void modify_pump_duty_cycle(uint8_t duty_cycle);

#endif // PWM_PUMP_H