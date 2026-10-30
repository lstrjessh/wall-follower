#pragma once
#include <avr/io.h>

// Motor PWM pins
#define LEFT_EN_PIN_OC2A   PB3  // D11 (OC2A)
#define RIGHT_EN_PIN_OC2B  PD3  // D3  (OC2B)

// Motor direction pins
#define LEFT_IN1_PIN       PD7  // D7
#define LEFT_IN2_PIN       PD6  // D6
#define RIGHT_IN3_PIN      PD5  // D5
#define RIGHT_IN4_PIN      PD4  // D4

// Servo pin
#define SERVO_PIN_OC1B     PB2  // D10 (OC1B)

// Ultrasonic pins
#define TRIG_PIN           PB4  // D12
#define ECHO_PIN           PD2  // D2

// Indicator LEDs
#define LED_FOLLOWING_PIN  PD0  // D0 - Wall following (normal operation)
#define LED_CORRECTING_PIN PD1  // D1 - Correcting (too close to wall)
#define LED_WALL_LOST_PIN  PD2  // D2 - Wall lost (searching for wall)

// Buzzer
#define BUZZER_PIN         PB0  // D8 - Sound indication




