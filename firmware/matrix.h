#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void (*matrix_callback_t)(uint8_t num, bool state);

void MATRIX_init(uint8_t rows, const uint8_t row_nums_[],
		uint8_t cols, const uint8_t col_nums_[],
		const uint8_t *matrix_, matrix_callback_t callback_);
bool MATRIX_scan();
