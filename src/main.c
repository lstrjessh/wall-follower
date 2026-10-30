#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include "board.h"
#include "motor.h"
#include "ultrasonic.h"
#include "servo.h"
#include "indicator.h"

// Servo angles
#define RIGHT_POS           30   // Look at right wall
#define CENTER_POS          100  // Look straight ahead

// Distance thresholds (cm)
#define DESIRED_DISTANCE_CM 25.4f  // Target: 10 inches from wall
#define TOO_CLOSE_CM        15.24f // Too close: 6 inches
#define DISTANCE_TOLERANCE  3.0f
#define OBSTACLE_TH_CM      25.4f  // Front obstacle: 10 inches
#define WALL_LOST_TH_CM     38.1f  // Wall lost: 15 inches

// Motor speeds (80-180)
#define BASE_SPEED          130
#define MIN_SPEED           80
#define MAX_SPEED           180
#define SPEED_CLAMP         40 // clamp adjustments

// Timing - ISR
#define SERVO_STABILIZE_MS  150
#define MEASUREMENT_SAMPLES 2
#define SAMPLE_DELAY_MS     15
#define LOOP_DELAY_MS       50

// State machine
typedef enum {
    STATE_CHECK_FRONT,
    STATE_CHECK_RIGHT
} RobotState;

// Take multiple ultrasonic readings and average them
static float take_precise_measurement_cm(void) {
    float total = 0.0f;
    uint8_t valid = 0;
    for (uint8_t i = 0; i < MEASUREMENT_SAMPLES; i++) {
        float d = ultrasonic_measure_distance_cm();
        if (d > 0.0f && d < 400.0f) { total += d; valid++; }
        _delay_ms(SAMPLE_DELAY_MS);
    }
    return valid > 0 ? total / valid : 100.0f;
}

static float previous_error = 0.0f;

// PD controller gains
#define KP 2.0f   // Proportional gain
#define KD 0.8f   // Derivative gain (dampens oscillations)

// Adjust motor speeds based on distance from wall
static void follow_wall(float right_distance_cm) {
    indicator_clear_all();
    indicator_set(INDICATOR_FOLLOWING, true);
    
    float error = right_distance_cm - DESIRED_DISTANCE_CM;
    float derivative = error - previous_error;
    previous_error = error;
    
    // PD control: respond to error and its rate of change
    int16_t speed_adjust = (int16_t)(KP * error + KD * derivative);
    if (speed_adjust > SPEED_CLAMP) speed_adjust = SPEED_CLAMP;
    if (speed_adjust < -SPEED_CLAMP) speed_adjust = -SPEED_CLAMP;
    
    int16_t left = BASE_SPEED - speed_adjust;
    int16_t right = BASE_SPEED + speed_adjust;
    
    if (left < MIN_SPEED) left = MIN_SPEED;
    if (left > MAX_SPEED) left = MAX_SPEED;
    if (right < MIN_SPEED) right = MIN_SPEED;
    if (right > MAX_SPEED) right = MAX_SPEED;

    motor_move_forward((uint8_t)left, (uint8_t)right);
    _delay_ms(150);
    motor_stop();
}

// Turn left to move away from wall when too close
static void correct_from_wall(void) {
    indicator_clear_all();
    indicator_set(INDICATOR_CORRECTING, true);
    buzzer_set(BUZZER_SHORT_BEEP);
    
    PORTD |= _BV(LEFT_IN2_PIN) | _BV(RIGHT_IN3_PIN);
    PORTD &= ~(_BV(LEFT_IN1_PIN) | _BV(RIGHT_IN4_PIN));
    motor_set_speeds(MIN_SPEED, MIN_SPEED + 20);
    _delay_ms(250);
    motor_stop();
    previous_error = 0.0f;
}

static void avoid_obstacle(void) {
    indicator_clear_all();
    indicator_set(INDICATOR_CORRECTING, true);
    buzzer_set(BUZZER_DOUBLE_BEEP);
    
    motor_turn_left(MIN_SPEED + 8);
    _delay_ms(300);
    motor_stop();
    _delay_ms(50);
    motor_move_forward(MIN_SPEED, MIN_SPEED);
    _delay_ms(150);
    motor_stop();
}

// Move forward then turn right to search for wall
static void wall_lost(void) {
    indicator_clear_all();
    indicator_set(INDICATOR_WALL_LOST, true);
    buzzer_set(BUZZER_CONTINUOUS);
    
    motor_move_forward(MIN_SPEED + 20, MIN_SPEED + 20);
    _delay_ms(200);
    motor_stop();
    _delay_ms(50);
    motor_turn_right(MIN_SPEED);
    _delay_ms(150);
    motor_stop();
    buzzer_stop();
    previous_error = 0.0f;
}

int main(void) {
    motor_pins_init();
    motor_pwm_init();
    ultrasonic_init();
    servo_init();
    indicator_init();

    // Center servo and start
    servo_write_degrees(CENTER_POS);
    _delay_ms(1000);

    sei();

    RobotState current = STATE_CHECK_FRONT;
    float front_distance = 0.0f;
    float right_distance = 0.0f;

    motor_stop();
    _delay_ms(500);

    for (;;) {
        switch (current) {
            case STATE_CHECK_FRONT:
                servo_write_degrees(CENTER_POS);
                _delay_ms(SERVO_STABILIZE_MS);
                front_distance = take_precise_measurement_cm();
                
                if (front_distance < OBSTACLE_TH_CM) {
                    avoid_obstacle();
                    current = STATE_CHECK_FRONT;
                } else {
                    current = STATE_CHECK_RIGHT;
                }
                break;

            case STATE_CHECK_RIGHT:
                servo_write_degrees(RIGHT_POS);
                _delay_ms(SERVO_STABILIZE_MS);
                right_distance = take_precise_measurement_cm();

                if (right_distance > WALL_LOST_TH_CM) {
                    wall_lost();
                    current = STATE_CHECK_FRONT;
                } else if (right_distance < TOO_CLOSE_CM) {
                    correct_from_wall();
                    // Check front to avoid getting stuck in corners
                    servo_write_degrees(CENTER_POS);
                    _delay_ms(SERVO_STABILIZE_MS);
                    front_distance = take_precise_measurement_cm();
                    if (front_distance < OBSTACLE_TH_CM) {
                        avoid_obstacle();
                    }
                    current = STATE_CHECK_FRONT;
                } else {
                    follow_wall(right_distance);
                    current = STATE_CHECK_FRONT;
                }
                break;

            default:
                current = STATE_CHECK_FRONT;
                break;
        }

        _delay_ms(LOOP_DELAY_MS);
    }
}


