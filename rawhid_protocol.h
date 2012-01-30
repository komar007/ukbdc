#pragma once

#include "rawhid.h"

#define MESSAGE_DFU	0x00

struct RAWHID_packet {
	uint8_t header;
	uint8_t payload[RAWHID_SIZE - 1];
};
