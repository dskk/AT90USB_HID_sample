#pragma once
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define LSB(n) (n & 255)
#define MSB(n) ((n >> 8) & 255)

typedef void (*func_ptr_t)(void);
