#pragma once

#include "usb.h"
#include <avr/pgmspace.h>

/* [Device configuration section] ------------------------------------------ */

#define STR_MANUFACTURER	L"komar"
#define STR_PRODUCT		L"komar board 0.1"

#define VENDOR_ID		0x16C0
#define PRODUCT_ID		0x047C

#define NUM_ENDPOINTS		4

#define ENDPOINT0_SIZE		32

/* [/Device configuration section] ------------------------------------------ */

/* [Inferface configuration section] --------------------------------------- */

/* Keyboard interface */
#define KEYBOARD_INTERFACE	0

#define KEYBOARD_ENDPOINT	1
#define KEYBOARD_SIZE		32
#define KEYBOARD_INTERVAL	1

/* RAWHID interface */
#define RAWHID_INTERFACE	1

#define RAWHID_TX_ENDPOINT	2
#define RAWHID_TX_SIZE		64	// transmit packet size
#define RAWHID_TX_INTERVAL	2	// max # of ms between transmit packets

#define RAWHID_RX_ENDPOINT	3
#define RAWHID_RX_SIZE		64	// receive packet size
#define RAWHID_RX_INTERVAL	2	// max # of ms between receive packets


/* [/Inferface configuration section] -------------------------------------- */

/* [API section] ----------------------------------------------------------- */

#define NUM_INTERFACE_REQUEST_HANDLERS		2
#define NUM_ENDPOINT_INTERRUPT_HANDLERS		0
#define NUM_SOF_HANDLERS			2

extern struct endpoint_config PROGMEM endpoint_configs[NUM_ENDPOINTS];
extern struct interface_request_handler iface_req_handlers[];
extern struct endpoint_interrupt_handler PROGMEM endpoint_int_handlers[];
extern struct sof_handler sof_handlers[];

/* [/API section] ---------------------------------------------------------- */
