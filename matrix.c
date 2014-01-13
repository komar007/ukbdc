/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*! \file matrix.c
 * implementation of module MATRIX
 */

#include "matrix.h"
#include "aux.h"
#include "io.h"

#include <util/delay.h>
#include <stdlib.h>
#include <string.h> /* memset */

static const uint8_t *matrix;
static uint8_t *states;
static const uint8_t *row_nums;
static const uint8_t *col_nums;
static int nrows, ncols;
static matrix_callback_t callback;

/*! Retrieves saved state of a key
 * \param row key row number
 * \param col key column number
 * \return `true` if the key is pressed, `false` otherwise
 */
bool is_pressed(uint8_t row, uint8_t col)
{
	const uint8_t pos = row*ncols + col;
	const uint8_t byte = pos / 8;
	const uint8_t bit = pos & 0x07;
	return (states[byte] >> bit) & 0x01;
}

/*! Sets real key state to memory
 * \param row key row number
 * \param col key column number
 * \param state key state
 */
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

/* if rows == 0, no rows will be multiplexed, but cols inputs will be read as
 * a one-row keyboard matrix */
void MATRIX_init(uint8_t rows, const uint8_t row_nums_[],
		uint8_t cols, const uint8_t col_nums_[],
		const uint8_t *matrix_, matrix_callback_t callback_)
{
	nrows = rows;
	ncols = cols;
	/* to assert that state array will be created properly for non-matrix
	 * keyboards... */
	if (rows == 0)
		rows = 1;
	matrix = matrix_;
	row_nums = row_nums_;
	col_nums = col_nums_;
	states = malloc(divceil(rows*cols, 8));
	memset(states, 0, divceil(rows*cols, 8));
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
	if (nrows == 0) {
		for (uint8_t j = 0; j < ncols; ++j) {
			bool state = !IO_get(col_nums[j]);
			if (state == is_pressed(0, j))
				continue;
			changed = true;
			set_state(0, j, state);
			callback(matrix[j], state);
		}
	}
	return changed;
}
