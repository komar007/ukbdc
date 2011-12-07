#include "rawhid.h"

#include <avr/interrupt.h>

// send a packet, with timeout
int8_t RAWHID_send(const uint8_t *buffer, uint8_t timeout)
{
	uint8_t intr_state;

	// if we're not online (enumerated and configured), error
	if (!USB_get_configuration()) return -1;
	intr_state = SREG;
	cli();
	uint8_t tx_timeout_count = timeout;
	UENUM = RAWHID_TX_ENDPOINT;
	// wait for the FIFO to be ready to accept data
	while (1) {
		if (UEINTX & (1<<RWAL)) break;
		SREG = intr_state;
		if (tx_timeout_count == 0) return 0;
		if (!USB_get_configuration()) return -1;
		intr_state = SREG;
		cli();
		UENUM = RAWHID_TX_ENDPOINT;
	}
	for (uint8_t i = 0; i < RAWHID_TX_SIZE; ++i)
		UEDATX = *buffer++;
	// transmit it now
	UEINTX = 0x3A;
	SREG = intr_state;
	return RAWHID_TX_SIZE;
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
			USB_ack_OUT();
			len -= n;
		} while (len);
		USB_wait_IN();
		USB_flush_IN();
	}
	return true;
}


void RAWHID_handle_rx_endpoint_interrupt(uint8_t flags)
{
	/*USB_ack_OUT();
	for (uint8_t i = 0; i < RAWHID_RX_SIZE; ++i)
		;
		*/
}
