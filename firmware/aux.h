#pragma once

#define ARR_SZ(a) (sizeof(a) / sizeof(*(a)))
#define LSB(n) (n & 0xff)
#define MSB(n) ((n >> 8) & 0xff)

#define offsetof(type, member)  __builtin_offsetof (type, member)

#define get_pgm_struct_field(addr, field) ( \
	(sizeof(((typeof(*(addr))*)0)->field) == 1) ? \
		pgm_read_byte((uint8_t*)(addr) + offsetof(typeof(*(addr)), field)) \
	: (sizeof(((typeof(*(addr))*)0)->field) == 2) ? \
		pgm_read_word((uint8_t*)(addr) + offsetof(typeof(*(addr)), field)) \
	: \
		pgm_read_dword((uint8_t*)(addr) + offsetof(typeof(*(addr)), field)) \
	)
