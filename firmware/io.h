#pragma once

#include "platforms.h"
#ifdef PLATFORM_alpha
	#include "hc595.h"
#endif

#include <avr/io.h>
#include <stdbool.h>

#define OUTPUT	true
#define INPUT	false

bool IO_get(uint8_t pin);
void IO_set(uint8_t pin, bool val);
void IO_config(uint8_t pin, bool dir);
