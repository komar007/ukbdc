#include "rawhid_protocol.h"
#include "layout.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <stdlib.h>
#include <string.h>

/* UKBDC RAWHID PROTOCOL
 * =====================
 *
 * ukbdc uses a rawhid based asynchronous protocol to support configuration of
 * the keyboard using PC-side software.
 *
 * The protocol uses messages to send data to device or host.
 * Each message is divided into packets whose size depends on the hardware
 * capabilities and USB configuration and may be 8, 16, 32 or 64 bytes.
 * A message always starts at the beginning of packet and may be continued by
 * continuation packets. If a message ends in the middle of a packet, the
 * remaining space in the pakcet is filled with irrelevant data.
 *
 * Packet
 * ------
 *  The packet format is the same for packets sent by the device and the host.
 *  A packet consists of a 1-byte header and payload. The header has the
 *  following format:
 *  MSB                LSB
 *  T C6 C5 C4 C3 C2 C1 C0
 *  T - type of packet:
 *    0 - beginning of message if C6...C0 = 0,
 *        special packet otherwise
 *    1 - continuation of previous message, C6...C0 carry continuation packet
 *    number - the first continuation packet (second packet of message) has
 *    number 1. The packet following packet number 127 has number 0.
 *  The rest of a packet is the payload which contains some part of a message
 *  or special data in case of special packet.
 *  Special packets may be sent at any time, including between two packets of a
 *  single message.
 *
 *  Special packet types (T = 0)
 *  C6...C0  description
 *  0x01     interrupt current message
 *
 * Message
 * -------
 *  A message is a vector of data send to device or host. It consists of a
 *  1-byte header which describes message type and optional data
 *  Host to Device:
 *  type description                             arguments
 *  0x00 enter DFU mode                          none
 *  0x01 set keyboard layout                     size (2 bytes)
 *                                               binary layout (size bytes)
 *
 *  Device to Host:
 */


void RAWHID_task()
{
	struct RAWHID_packet buf;
	if (RAWHID_recv(&buf)) {
		switch (buf.header) {
		case MESSAGE_DFU: {
			TIMSK0=0;
			USB_close();
			__asm__("jmp 0x3800");
			break;
		} case MESSAGE_SET_LAYOUT: {
			int n = buf.payload[0] + (buf.payload[1] << 8);
			uint8_t *ptr = malloc(sizeof(uint8_t) * n);
			uint8_t *ptr1 = ptr;
			while (n > 0) {
				while (!RAWHID_recv(&buf))
					;
				uint8_t nrecv = n > RAWHID_SIZE ? RAWHID_SIZE : n;
				memcpy(ptr1, &buf, nrecv);
				ptr1 += nrecv;
				n -= RAWHID_SIZE;
			}
			LAYOUT_set(ptr);
			break;
		}
		}
	}
}
