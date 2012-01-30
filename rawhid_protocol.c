#include "rawhid_protocol.h"

#include <avr/interrupt.h>
#include <avr/power.h>

/* UKBDC RAWHID PROTOCOL
 * =====================
 *
 * ukbdc uses a rawhid based asynchronous protocol to support configuration of
 * the keyboard using PC-side software.
 *
 * The protocol uses messages to send data to device or host.
 * Each message is divided into packets whose size depends on the hardware
 * capabilities and USB configuration and may be 8, 16, 32 or 64 bytes.
 * Each message starts at the beginning of packet and if the size of message is
 * not divisable by the size of packet, then the last packet is padded with
 * irrelevant data.
 *
 * Packet
 * ------
 *  The packet format is the same for packets sent by the device and the host.
 *  A packet consists of 1-byte header and payload. The header has the
 *  following format:
 *  MSB                LSB
 *  T C6 C5 C4 C3 C2 C1 C0
 *  T - type of packet:
 *    0 - beginning of message, C6...C0 define message type
 *    1 - continuation of previous message, C6...C0 carry continuation packet
 *    number - the first continuation packet (second packet of message) has
 *    number 0. The packet following packet number 127 has number 0.
 *
 * Message
 * -------
 *  Host to Device:
 *  type description                             arguments
 *  0x00 enter DFU mode                          none
 *
 *  Device to Host:
 */

void RAWHID_task()
{
	struct RAWHID_packet buf;
	if (RAWHID_recv((uint8_t*)&buf)) {
		switch (buf.header) {
		case MESSAGE_DFU:
			cli();
			TIMSK0=0;
			USB_close();
			clock_prescale_set(clock_div_8);
			__asm__("jmp 0x3800");
			break;
		}
	}
}
