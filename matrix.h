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


/*! \defgroup MATRIX
 *
 * \brief Reading keyboard matrices
 *
 * This module implements matrix keyboard support. It scans a matrix by
 * setting one of the row pins to 0 at a time and reading the column
 * pins. The pins are properly configured each time the matrix is scanned, but
 * their configuration is not restored afterwards. The ouput of the row
 * currently scanned is set to 0, while the outputs of other rows are set to
 * Hi-Z.
 *
 * It is also possible to read the state of a number of keys connected to
 * microcontroller's pins, without the concept of a matrix. In order to
 * achieve this, MATRIX_init() must be called with `rows` set to `0`.
 *
 * The keys are identified by numbers as an extra level of indirection, so a
 * mapping between matrix coordinates and key numbers has to be provided. This
 * way after a change in the matrix layout or even rearrangement of the keys
 * it is enough to change the mapping array.
 *
 * @{
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/*! A function of this type is called each time the key matrix changes state
 *
 * \param key_num the number of the key which changed state
 * \param state the current state of the key
 */
typedef void (*matrix_callback_t)(uint8_t key_num, bool state);

/*! Initializes the MATRIX module. This function must be called before any
 * other function in this module.
 *
 * \param rows number of rows in the matrix; a special value of `0` is also
 * accepted which means there are no rows to control - in this case one row is
 * assumed, and there is no multiplexing
 * \param row_nums_ an array of size `rows` of output pins for each row
 * \param cols number of columns in the matrix
 * \param col_nums_ an array of size cols of input pins for each column
 * \param matrix an array of size `rows*cols` representing the mapping between
 * physical keys and their numbers, stored in row-major order, such that
 * `matrix[cols*i + j]` contains the number assigned to the key in row `i` and
 * column `j`
 * \param callback_ the function to be called when a key changes state
 */
void MATRIX_init(uint8_t rows, const uint8_t row_nums_[],
		uint8_t cols, const uint8_t col_nums_[],
		const uint8_t *matrix_, matrix_callback_t callback_);

/*! Performs a single matrix scan and launches callback for each key which
 * changed state since the last scan */
bool MATRIX_scan();

/*! @} */
