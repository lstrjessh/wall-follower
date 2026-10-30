#include <avr/io.h>
#include "board.h"
#include "motor.h"

void motor_pins_init(void) {
    // Direction pins as outputs
    DDRD |= _BV(LEFT_IN1_PIN) | _BV(LEFT_IN2_PIN) | _BV(RIGHT_IN3_PIN) | _BV(RIGHT_IN4_PIN);
    // PWM pins as outputs
    DDRB |= _BV(LEFT_EN_PIN_OC2A);
    DDRD |= _BV(RIGHT_EN_PIN_OC2B);
}

void motor_pwm_init(void) {
    // Timer2 Fast PWM, prescaler 64 (~976 Hz)
    TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
    TCCR2B = _BV(CS22);
    OCR2A = 0;
    OCR2B = 0;
}

void motor_set_speeds(uint8_t left, uint8_t right) {
    OCR2A = left;   // Left motor: D11
    OCR2B = right;  // Right motor: D3
}

void motor_stop(void) {
    PORTD &= ~(_BV(LEFT_IN1_PIN) | _BV(LEFT_IN2_PIN) | _BV(RIGHT_IN3_PIN) | _BV(RIGHT_IN4_PIN));
    motor_set_speeds(0, 0);
}

void motor_move_forward(uint8_t left, uint8_t right) {
    PORTD |= _BV(LEFT_IN1_PIN) | _BV(RIGHT_IN3_PIN);
    PORTD &= ~(_BV(LEFT_IN2_PIN) | _BV(RIGHT_IN4_PIN));
    motor_set_speeds(left, right);
}

void motor_turn_left(uint8_t speed) {
    PORTD |= _BV(LEFT_IN2_PIN) | _BV(RIGHT_IN3_PIN);
    PORTD &= ~(_BV(LEFT_IN1_PIN) | _BV(RIGHT_IN4_PIN));
    motor_set_speeds(speed, speed);
}

void motor_turn_right(uint8_t speed) {
    PORTD |= _BV(LEFT_IN1_PIN) | _BV(RIGHT_IN4_PIN);
    PORTD &= ~(_BV(LEFT_IN2_PIN) | _BV(RIGHT_IN3_PIN));
    motor_set_speeds(speed, speed);
}


