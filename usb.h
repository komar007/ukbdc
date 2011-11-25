#pragma once

#include "usb_hardware.h"

/* [FLAGS FOR bmRequestType] */
/* Request direction */
#define MASK_REQ_DIR		0x80
#define HOST_TO_DEVICE		0x00
#define DEVICE_TO_HOST		0x80
/* Request type */
#define MASK_REQ_TYPE		0x60
#define STANDARD		0x00
#define CLASS			0x20
#define VENDOR			0x40
/* Request recipient */
#define MASK_REQ_RCPT		0x1F
#define DEVICE			0x00
#define INTERFACE		0x01
#define ENDPOINT		0x02
#define OTHER			0x03

#define MASK_ALL 		(MASK_REQ_DIR | MASK_REQ_TYPE | MASK_REQ_RCPT)
/* [/FLAGS FOR bmRequestType] */

/* standard control endpoint request types */
#define GET_STATUS			0
#define CLEAR_FEATURE			1
#define SET_FEATURE			3
#define SET_ADDRESS			5
#define GET_DESCRIPTOR			6
#define GET_CONFIGURATION		8
#define SET_CONFIGURATION		9
#define GET_INTERFACE			10
#define SET_INTERFACE			11
// HID (human interface device)
#define HID_GET_REPORT			1
#define HID_GET_IDLE			2
#define HID_GET_PROTOCOL		3
#define HID_SET_REPORT			9
#define HID_SET_IDLE			10
#define HID_SET_PROTOCOL		11


struct setup_packet {
	uint8_t  bmRequestType;
	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
};

struct interface_request_handler {
	uint16_t iface_number;
	void (*function)(struct setup_packet*);
};

static inline bool request_type(struct setup_packet *s,
		uint8_t mask, uint8_t flags)
{
	return (s->bmRequestType & mask) == flags;
}
static inline bool request(struct setup_packet *s, uint8_t req)
{
	return s->bRequest == req;
}

static inline void USB_control_write_complete_status_stage()
{
	USB_wait_IN();
	/* send Zero Length Packet */
	USB_flush_IN();
}

static inline void USB_control_read_complete_status_stage()
{
	/* wait for Zero Length Packet from host */
	USB_wait_OUT();
	/* acknowledge it */
	USB_ack_OUT();
}

