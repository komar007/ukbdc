#pragma once

#include "rawhid.h"

/* Packet types */
#define PING                    0x00
#define PONG                    0x01
#define MSG_START               0x02
#define MSG_CONT                0x03
#define RESET_PROTO             0x04

/* Message types */
#define MESSAGE_DFU		0x00
#define MESSAGE_SET_LAYOUT	0x01

struct RAWHID_packet {
	uint8_t header;
	uint8_t payload[RAWHID_SIZE - 1];
};

void RAWHID_PROTOCOL_task();
void RAWHID_PROTOCOL_handle_packet(uint8_t __attribute__((unused)) flags);
