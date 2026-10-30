#pragma once
#include <stdint.h>

// Initialize Timer1 and servo output pin for 50 Hz control
void servo_init(void);

// Write servo angle in degrees (0..180)
void servo_write_degrees(uint8_t degrees);


