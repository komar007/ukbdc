#include "rawhid.h"

#include <avr/interrupt.h>

uint8_t rawhid_rx_buffer[RAWHID_RX_SIZE];

bool RAWHID_send(const uint8_t *buffer)
{
	if (!USB_get_configuration())
		return false;
	USB_set_endpoint(RAWHID_TX_ENDPOINT);
	if (bit_is_clear(UEINTX, TXINI))
		return false;
	USB_IN_write_buffer(buffer, RAWHID_TX_SIZE);
	USB_flush_IN();
	return true;
}

bool RAWHID_handle_control_request(struct setup_packet *s)
{
	uint8_t i, len, n;
	if (s->bmRequestType == 0xA1 && s->bRequest == HID_GET_REPORT) {
		len = RAWHID_TX_SIZE;
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
		len = RAWHID_RX_SIZE;
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


void RAWHID_handle_rx_endpoint_interrupt(uint8_t flags)
{
	if (flags & _BV(RXOUTI)) {
		USB_OUT_read_buffer(rawhid_rx_buffer, RAWHID_RX_SIZE);
		USB_flush_OUT();
	}
}
