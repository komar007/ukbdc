#pragma once

#include <stdint.h>
#include <stdlib.h>

void LAYOUT_set(uint8_t *layout);
uint8_t LAYOUT_get_scancode(uint8_t layer, uint8_t keycode);
