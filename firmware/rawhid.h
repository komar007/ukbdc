#pragma once

#include "usb.h"
#include "usb_config.h"

#define RAWHID_USAGE_PAGE	0xFFAB	// recommended: 0xFF00 to 0xFFFF
#define RAWHID_USAGE		0x0200	// recommended: 0x0100 to 0xFFFF

bool RAWHID_handle_control_request(struct setup_packet *s);
bool RAWHID_send(const void *buffer);
bool RAWHID_recv(void *buffer);
