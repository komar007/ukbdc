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
#define RAWHID_SIZE		64	// transmit/receive packet size
#define RAWHID_TX_INTERVAL	2	// max # of ms between transmit packets

#define RAWHID_RX_ENDPOINT	3
#define RAWHID_RX_INTERVAL	2	// max # of ms between receive packets


/* [/Inferface configuration section] -------------------------------------- */

typedef bool (*interface_request_handler_fun)(struct setup_packet*);
/* A standard request handler takes a setup_packet and returns true if the
 * request was processed or false if there was an error or the request is not
 * supported */
struct interface_request_handler {
	uint16_t iface_num;
	interface_request_handler_fun f;
};

/* A SOF handler is called each time a SOF comes */
struct sof_handler {
	void (*f)();
};

typedef void (*endpoint_interrupt_handler_fun)(uint8_t flags);
/* An endpoint interrupt handler is called each time there is an interrupt on
 * the endpoint. The interrupt masks per endpoint are configured in struct
 * endpoint_config (int_flags field) */
struct endpoint_interrupt_handler {
	uint8_t endpoint_num;
	endpoint_interrupt_handler_fun f;
};

/* [API section] ----------------------------------------------------------- */

#define NUM_INTERFACE_REQUEST_HANDLERS		2
#define NUM_ENDPOINT_INTERRUPT_HANDLERS		1
#define NUM_SOF_HANDLERS			2

extern struct endpoint_config PROGMEM endpoint_configs[NUM_ENDPOINTS];
extern struct interface_request_handler iface_req_handlers[];
extern struct endpoint_interrupt_handler PROGMEM endpoint_int_handlers[];
extern struct sof_handler sof_handlers[];

/* [/API section] ---------------------------------------------------------- */
