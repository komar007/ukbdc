#pragma once

#define ARR_SZ(a) (sizeof(a) / sizeof(*(a)))
#define LSB(n) (n & 0xff)
#define MSB(n) ((n >> 8) & 0xff)
