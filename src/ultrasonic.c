#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "board.h"
#include "ultrasonic.h"

static volatile uint16_t echo_start = 0;
static volatile uint16_t echo_end = 0;
static volatile uint8_t echo_captured = 0;

static inline void ultrasonic_trigger_pulse(void) {
    PORTB &= ~_BV(TRIG_PIN);
    _delay_us(2);
    PORTB |= _BV(TRIG_PIN);
    _delay_us(10);
    PORTB &= ~_BV(TRIG_PIN);
}

void ultrasonic_init(void) {
    // TRIG as output, ECHO as input
    DDRB |= _BV(TRIG_PIN);
    DDRB &= ~_BV(ECHO_PIN);

    // Use pin-change interrupt on PCINT5 (PB5) to detect echo edges
    // Timer1 timestamps edges at 0.5us resolution (shared with servo)
    PCICR |= _BV(PCIE0);     // Enable PCINT[7:0]
    PCMSK0 |= _BV(PCINT5);   // Enable PB5 interrupt
}

ISR(PCINT0_vect) {
    uint8_t high = (PINB & _BV(ECHO_PIN)) != 0;
    uint16_t now = TCNT1;  // Timestamp at 0.5us resolution
    if (high) {
        echo_start = now;
        echo_captured = 0;
    } else {
        echo_end = now;
        echo_captured = 1;
    }
}

float ultrasonic_measure_distance_cm(void) {
    echo_captured = 0;
    ultrasonic_trigger_pulse();

    // Wait for echo with 30ms timeout
    uint16_t waited_ms = 0;
    while (!echo_captured && waited_ms < 30) {
        _delay_ms(1);
        waited_ms++;
    }
    if (!echo_captured) return 400.0f;

    // Unsigned subtraction handles wrap-around automatically
    uint16_t ticks = echo_end - echo_start;

    // Each tick = 0.5us (Timer1 prescaler 8)
    float duration_us = (float)ticks * 0.5f;
    float distance = duration_us * 0.0343f * 0.5f;
    if (distance > 400.0f) distance = 400.0f;
    return distance;
}


