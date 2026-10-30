#include <avr/io.h>
#include "board.h"
#include "servo.h"

void servo_init(void) {
    DDRB |= _BV(SERVO_PIN_OC1B);
    // Timer1 Fast PWM: 50Hz (20ms period), prescaler 8 (0.5us tick)
    TCCR1A = _BV(COM1B1) | _BV(WGM11);
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11);
    ICR1 = 39999;   // 20ms period
    OCR1B = 3000;   // Center position (1.5ms)
}

void servo_write_degrees(uint8_t degrees) {
    if (degrees > 180) degrees = 180;
    // Arduino standard: 544-2400us for 0-180°
    uint16_t pulse_us = 544 + ((uint32_t)degrees * 1856) / 180;
    uint16_t ocr = (uint16_t)(pulse_us * 2U);  // Convert to 0.5us ticks
    OCR1B = ocr;
}


