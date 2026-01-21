#ifndef PWM_PUMP_H
#define PWM_PUMP_H

#include "driver/gpio.h"
#include "driver/ledc.h"

#define PWM_WATER_MOTOR_PIN         GPIO_NUM_12
#define PWM_FERTILIZER_MOTOR_PIN    GPIO_NUM_13
#define PWM_TIMER                   LEDC_TIMER_0
#define PWM_MODE                    LEDC_LOW_SPEED_MODE
#define PWM_WATER_CHANNEL           LEDC_CHANNEL_0
#define PWM_FERTILIZER_CHANNEL      LEDC_CHANNEL_1
#define PWM_RESOLUTION              LEDC_TIMER_8_BIT
#define PWM_FREQUENCY_HZ            4000
#define PWM_DUTY_100_PERCENT        255
#define PWM_DUTY_50_PERCENT         128
#define PWM_DUTY_0_PERCENT          0
#define FERTLIZER                   true
#define WATER                       false


// PWM Initialization and Configuration Functions
void pwm_pump_init(bool component);
void modify_pump_duty_cycle(bool component, uint8_t duty_cycle);

#endif // PWM_PUMP_H