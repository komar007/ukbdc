#include "rawhid.h"

#include <avr/interrupt.h>

/* [Callbacks section] ----------------------------------------------------- */
bool RAWHID_handle_control_request(struct setup_packet *s)
{
	uint8_t i, len, n;
	if (s->bmRequestType == 0xA1 && s->bRequest == HID_GET_REPORT) {
		len = RAWHID_SIZE;
		do {
			// wait for host ready for IN packet
			do {
				i = UEINTX;
			} while (!(i & ((1<<TXINI)|(1<<RXOUTI))));
			if (i & (1<<RXOUTI)) return false;	// abort
			// send IN packet
			n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
			for (i = n; i; i--) {
				// just send zeros
				UEDATX = 0;
			}
			len -= n;
			USB_flush_IN();
		} while (len || n == ENDPOINT0_SIZE);
	}
	if (s->bmRequestType == 0x21 && s->bRequest == HID_SET_REPORT) {
		len = RAWHID_SIZE;
		do {
			n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
			USB_wait_OUT();
			// ignore incoming bytes
			USB_flush_OUT();
			len -= n;
		} while (len);
		USB_wait_IN();
		USB_flush_IN();
	}
	return true;
}
/* [/Callbacks section] ---------------------------------------------------- */

/* [API section] ----------------------------------------------------------- */
bool RAWHID_send(const void *buffer)
{
	if (!USB_get_configuration())
		return false;
	uint8_t sreg = SREG;
	cli();
	USB_set_endpoint(RAWHID_TX_ENDPOINT);
	if (!USB_IN_ready()) {
		SREG = sreg;
		return false;
	}
	USB_IN_write_buffer(buffer, RAWHID_SIZE);
	USB_flush_IN();
	SREG = sreg;
	return true;
}

bool RAWHID_recv(void *buffer)
{
	if (!USB_get_configuration())
		return false;
	uint8_t sreg = SREG;
	cli();
	USB_set_endpoint(RAWHID_RX_ENDPOINT);
	if (!USB_OUT_ready()) {
		SREG = sreg;
		return false;
	}
	USB_OUT_read_buffer(buffer, RAWHID_SIZE);
	USB_flush_OUT();
	SREG = sreg;
	return true;
}
/* [/API section] ---------------------------------------------------------- */
