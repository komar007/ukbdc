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

/*! \file aux.h
 * Auxilliary definitions and macros which don't fit anywhere else
 */

#pragma once

#include <stddef.h>

/*! Computes array size */
#define ARR_SZ(a) (sizeof(a) / sizeof(*(a)))
/*! Extracts the Less Significant Byte of a word */
#define LSB(n) (n & 0xff)
/*! Extracts the More Significant Byte of a word */
#define MSB(n) ((n >> 8) & 0xff)

/*! A macro used to read a struct field from program memory (supported field
 * sizes: 1, 2 and 4 bytes)
 * \param addr the address to the structure in flash
 * \param field the name of the structure's field to be read
 * \return the value of the field
 */
#define get_pgm_struct_field(addr, field) ( \
	(sizeof(((typeof(*(addr))*)0)->field) == 1) ? \
		pgm_read_byte((uint8_t*)(addr) + offsetof(typeof(*(addr)), field)) \
	: (sizeof(((typeof(*(addr))*)0)->field) == 2) ? \
		pgm_read_word((uint8_t*)(addr) + offsetof(typeof(*(addr)), field)) \
	: \
		pgm_read_dword((uint8_t*)(addr) + offsetof(typeof(*(addr)), field)) \
	)

/* evaluates to ceil(x / n) */
#define divceil(x, n) (((x) + (n) - 1) / (n))
#define min(x, y) ((x) < (y) ? (x) : (y))
