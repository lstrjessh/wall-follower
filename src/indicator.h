#pragma once
#include <stdint.h>
#include <stdbool.h>

// Indicator types
typedef enum {
    INDICATOR_FOLLOWING,   // D0 - Normal wall following
    INDICATOR_CORRECTING,  // D1 - Correcting position (too close)
    INDICATOR_WALL_LOST    // D2 - Wall lost (searching)
} IndicatorType;

// Buzzer patterns
typedef enum {
    BUZZER_OFF,
    BUZZER_SHORT_BEEP,     // Single short beep
    BUZZER_DOUBLE_BEEP,    // Two quick beeps
    BUZZER_CONTINUOUS      // Continuous tone
} BuzzerPattern;

// Initialize indicator LEDs and buzzer
void indicator_init(void);

// Set specific indicator LED on/off
void indicator_set(IndicatorType type, bool on);

// Turn off all indicator LEDs
void indicator_clear_all(void);

// Set buzzer pattern (non-blocking for short beeps)
void buzzer_set(BuzzerPattern pattern);

// Stop buzzer
void buzzer_stop(void);
