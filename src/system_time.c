#include <avr/io.h>
#include <avr/interrupt.h>
#include "system_time.h"

static volatile uint32_t system_millis = 0;

void time_init(void) {
    // Timer0 CTC mode: 1ms tick (16MHz / 64 / 250)
    TCCR0A = _BV(WGM01);
    TCCR0B = _BV(CS01) | _BV(CS00);  // Prescaler 64
    OCR0A = 249;
    TIMSK0 = _BV(OCIE0A);  // Enable interrupt
}

ISR(TIMER0_COMPA_vect) {
    system_millis++;
}

uint32_t millis(void) {
    uint32_t m;
    uint8_t sreg = SREG;
    cli();
    m = system_millis;
    SREG = sreg;
    return m;
}


