#pragma once

#include <stddef.h>

#define ARR_SZ(a) (sizeof(a) / sizeof(*(a)))
#define LSB(n) (n & 0xff)
#define MSB(n) ((n >> 8) & 0xff)

/* Macro used to read a struct field from program memory (supported field
 * sizes: 1, 2 and 4 bytes) */
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
