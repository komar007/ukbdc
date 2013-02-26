#include "matrix.h"
#include "aux.h"
#include "io.h"

#include <util/delay.h>
#include <stdlib.h>

static const uint8_t *matrix;
static uint8_t *states;
static const uint8_t *row_nums;
static const uint8_t *col_nums;
static int nrows, ncols;
static matrix_callback_t callback;

bool is_pressed(uint8_t row, uint8_t col)
{
	const uint8_t pos = row*ncols + col;
	const uint8_t byte = pos / 8;
	const uint8_t bit = pos & 0x07;
	return (states[byte] >> bit) & 0x01;
}
void set_state(uint8_t row, uint8_t col, bool state)
{
	const uint8_t pos = row*ncols + col;
	const uint8_t byte = pos / 8;
	const uint8_t bit = pos & 0x07;
	if (state)
		states[byte] |= (1 << bit);
	else
		states[byte] &= ~(1 << bit);
}

void MATRIX_init(uint8_t rows, const uint8_t row_nums_[],
		uint8_t cols, const uint8_t col_nums_[],
		const uint8_t *matrix_, matrix_callback_t callback_)
{
	nrows = rows;
	ncols = cols;
	matrix = matrix_;
	row_nums = row_nums_;
	col_nums = col_nums_;
	states = malloc(divceil(rows*cols, 8));
	callback = callback_;
}

bool MATRIX_scan()
{
	/* set all rows to Hi-Z */
	for (uint8_t i = 0; i < nrows; ++i) {
		IO_config(row_nums[i], INPUT);
		IO_set(row_nums[i], false);
	}
	/* set all columns to input with pull-ups */
	for (uint8_t i = 0; i < ncols; ++i) {
		IO_config(col_nums[i], INPUT);
		IO_set(col_nums[i], true);
	}
	bool changed = false;
	/* scan the matrix */
	for (uint8_t i = 0; i < nrows; ++i) {
		IO_config(row_nums[i], OUTPUT);
		_delay_us(1);
		for (uint8_t j = 0; j < ncols; ++j) {
			bool state = !IO_get(col_nums[j]);
			if (state == is_pressed(i, j))
				continue;
			changed = true;
			set_state(i, j, state);
			callback(matrix[i*ncols + j], state);
		}
		IO_config(row_nums[i], INPUT);
	}
	return changed;
}
