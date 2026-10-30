#pragma once
#include <stdint.h>

void motor_pins_init(void);
void motor_pwm_init(void);
void motor_set_speeds(uint8_t left, uint8_t right);
void motor_stop(void);
void motor_move_forward(uint8_t left, uint8_t right);
void motor_turn_left(uint8_t speed);
void motor_turn_right(uint8_t speed);


