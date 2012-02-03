#pragma once

#include "rawhid.h"

#define MESSAGE_DFU		0x00
#define MESSAGE_SET_LAYOUT	0x01

struct RAWHID_packet {
	uint8_t header;
	uint8_t payload[RAWHID_SIZE - 1];
};

void RAWHID_task();
