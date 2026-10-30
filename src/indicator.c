#include "indicator.h"
#include "board.h"
#include <avr/io.h>
#include <util/delay.h>

// Generate tone by toggling buzzer at given frequency
static void buzzer_tone(uint16_t frequency_hz, uint16_t duration_ms) {
    uint32_t period_us = 1000000UL / frequency_hz;
    uint32_t half_period_us = period_us / 2;
    uint32_t cycles = (uint32_t)frequency_hz * duration_ms / 1000;
    
    for (uint32_t i = 0; i < cycles; i++) {
        PORTB |= _BV(BUZZER_PIN);
        for (uint16_t d = 0; d < half_period_us; d++) _delay_us(1);
        PORTB &= ~_BV(BUZZER_PIN);
        for (uint16_t d = 0; d < half_period_us; d++) _delay_us(1);
    }
}

// Initialize indicator pins as outputs
void indicator_init(void) {
    // Set LED pins as outputs
    DDRD |= _BV(LED_FOLLOWING_PIN) | _BV(LED_CORRECTING_PIN) | _BV(LED_WALL_LOST_PIN);
    
    // Set buzzer pin as output
    DDRB |= _BV(BUZZER_PIN);
    
    // Turn off all indicators initially
    PORTD &= ~(_BV(LED_FOLLOWING_PIN) | _BV(LED_CORRECTING_PIN) | _BV(LED_WALL_LOST_PIN));
    PORTB &= ~_BV(BUZZER_PIN);
}

// Set specific indicator LED on/off
void indicator_set(IndicatorType type, bool on) {
    switch (type) {
        case INDICATOR_FOLLOWING:
            if (on) PORTD |= _BV(LED_FOLLOWING_PIN);
            else PORTD &= ~_BV(LED_FOLLOWING_PIN);
            break;
            
        case INDICATOR_CORRECTING:
            if (on) PORTD |= _BV(LED_CORRECTING_PIN);
            else PORTD &= ~_BV(LED_CORRECTING_PIN);
            break;
            
        case INDICATOR_WALL_LOST:
            if (on) PORTD |= _BV(LED_WALL_LOST_PIN);
            else PORTD &= ~_BV(LED_WALL_LOST_PIN);
            break;
    }
}

// Turn off all indicator LEDs
void indicator_clear_all(void) {
    PORTD &= ~(_BV(LED_FOLLOWING_PIN) | _BV(LED_CORRECTING_PIN) | _BV(LED_WALL_LOST_PIN));
}

// Set buzzer pattern
void buzzer_set(BuzzerPattern pattern) {
    switch (pattern) {
        case BUZZER_OFF:
            PORTB &= ~_BV(BUZZER_PIN);
            break;
            
        case BUZZER_SHORT_BEEP:
            // Single 100ms beep at 2kHz
            buzzer_tone(2000, 100);
            break;
            
        case BUZZER_DOUBLE_BEEP:
            // Two 80ms beeps at 2.5kHz with 50ms gap
            buzzer_tone(2500, 80);
            _delay_ms(50);
            buzzer_tone(2500, 80);
            break;
            
        case BUZZER_CONTINUOUS:
            // Start continuous 1.5kHz tone (needs manual stop)
            buzzer_tone(1500, 200);
            break;
    }
}

// Stop buzzer
void buzzer_stop(void) {
    PORTB &= ~_BV(BUZZER_PIN);
}
