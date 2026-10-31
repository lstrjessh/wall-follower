# Wall-Following Robot

Bare-metal C firmware for an ATmega328P robot that follows a right-hand wall using ultrasonic distance measurements and PD motor control. Written against AVR registers, without the Arduino framework.

## Features

- Alternating front and right-side measurements using a servo-mounted ultrasonic sensor.
- PD steering toward a target wall distance of 25.4 cm, with bounded motor speed corrections.
- Separate behaviors for obstacles, excessive wall proximity, and wall loss.
- Timer2 motor PWM, Timer1 servo PWM, and interrupt-based echo timing.
- LED and buzzer feedback for operating states.

## Stack

C, AVR-GCC, ATmega328P, AVRDUDE, and Make.

## Build

Install AVR-GCC, AVR binutils, Make, and AVRDUDE. The supplied Makefile targets an ATmega328P at 16 MHz.

```bash
make
make size
```

Build artifacts are written to `build/`. On Windows, use an environment providing the Unix shell commands used by the Makefile, such as MSYS2.

After reconciling the pin configuration with your wiring:

```bash
make flash PORT=COM5
```

Override `PORT` for your board. The default programmer is the Arduino bootloader protocol at 115200 baud.

## Source layout

| File | Responsibility |
| --- | --- |
| `src/main.c` | State machine, measurement averaging, PD control, and recovery behaviors |
| `src/board.h` | Pin assignments |
| `src/motor.c` | Direction control and motor PWM |
| `src/servo.c` | Servo pulse generation |
| `src/ultrasonic.c` | Trigger pulses and interrupt-based echo measurement |
| `src/indicator.c` | Status LEDs and buzzer patterns |
| `src/system_time.c` | Additional timing module, not included in the current Makefile |

## Configuration

Distance thresholds, controller gains, motor limits, and servo angles are defined in `src/main.c`. Pin assignments are in `src/board.h`.

## Current limitations

This repository preserves the supplied prototype firmware. Hardware operation and compilation have not been verified for this publication.

- The echo pin is declared as PD2 in `board.h`, but the ultrasonic driver reads PORTB and enables the PB5 pin-change interrupt. PD2 is also assigned to the wall-loss LED. Reconcile these assignments before flashing.
- Timer1 resets at its PWM top of 39999. Echo timing currently assumes ordinary 16-bit wraparound, so measurements spanning a timer reset need correction.
- The control loop uses blocking delays, and its derivative term is the change between successive errors rather than a time-normalized derivative.
- Sensor timeouts fall back to a large distance; review recovery behavior before operating the robot near obstacles.

A hardware demo, wiring diagram, and measured tracking performance are useful next additions.
