# Minimal AVR Makefile for Arduino Uno (ATmega328P @ 16 MHz)

MCU       := atmega328p
F_CPU     := 16000000UL
BAUD      := 9600

TARGET    := wall_follower
BUILD_DIR := build
SRC_DIR   := src
SRC       := \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/motor.c \
	$(SRC_DIR)/ultrasonic.c \
	$(SRC_DIR)/servo.c \
	$(SRC_DIR)/indicator.c

AVR_PREFIX ?=
CC        := $(AVR_PREFIX)avr-gcc
OBJCOPY   := $(AVR_PREFIX)avr-objcopy
AVRDUDE   := $(AVR_PREFIX)avrdude
OBJSIZE   := $(AVR_PREFIX)avr-size

CFLAGS    := -std=gnu99 -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Wall -Wextra -Werror=implicit-function-declaration
LDFLAGS   := -mmcu=$(MCU)

# Upload settings (override from command line if needed)
# Example: make flash PORT=COM5
PORT      ?= COM4
PROGRAMMER:= arduino
AVRDUDE_FLAGS := -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b 115200 -D

ELF := $(BUILD_DIR)/$(TARGET).elf
HEX := $(BUILD_DIR)/$(TARGET).hex

.PHONY: all clean flash size

all: $(HEX)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(ELF): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

flash: $(HEX)
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$(HEX):i

size: $(ELF)
	$(OBJSIZE) --mcu=$(MCU) --format=avr $<

clean:
	@if [ -d $(BUILD_DIR) ]; then rm -rf $(BUILD_DIR); fi


