#pragma once

#include "usb.h"
#include "usb_config.h"

#define RAWHID_USAGE_PAGE	0xFFAB	// recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE		0x0200	// recommended: 0x0100 to 0xFFFF

extern uint8_t rawhid_rx_buffer[RAWHID_RX_SIZE];

bool RAWHID_handle_control_request(struct setup_packet *s);
void RAWHID_handle_rx_endpoint_interrupt(uint8_t flags);
bool RAWHID_send(const uint8_t *buffer);
bool RAWHID_recv();
