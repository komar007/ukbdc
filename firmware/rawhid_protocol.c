#include "rawhid_protocol.h"
#include "layout.h"

#include <avr/interrupt.h>
#include <avr/power.h>
#include <stdlib.h>
#include <string.h>

/* UKBDC RAWHID PROTOCOL - draft
 * =====================
 *
 * Note: this is the documentation of the new protocol (in progress).
 * This file implements the old protocol!
 *
 * ukbdc uses a rawhid based asynchronous protocol to support configuration of
 * the keyboard using PC-side software.
 *
 * The protocol uses messages to send data to device or host.
 * Each message is divided into packets whose size depends on the hardware
 * capabilities and USB configuration and may be 8, 16, 32 or 64 bytes.
 * A message always starts at the beginning of packet and may be continued by
 * continuation packets. If a message ends in the middle of a packet, the
 * remaining space in the packet is filled with irrelevant data.
 * Packets may not only carry messages. Packets which do not carry a message
 * are called control packets. They must be self-contained and may be sent
 * during transmission of a message.
 *
 * Packet
 * ------
 *  The packet format is the same for packets sent by the device and the host.
 *  A packet consists of a 1-byte header and optional payload.
 *  The rest of a packet is the payload which contains some part of a message
 *  or control data in case of control packet.
 *  Control packets may be sent at any time, including between two packets of a
 *  single message.
 *  The header indicates packet type:
 *  0x00     ping - the side which receives this packet should respond with
 *                  pong, even in case of an ongoing transmission of a message
 *  0x01     pong - answer to the ping message
 *  0x02     message start - the first packet of a message; the next two bytes
 *                           are the size of the message in bytes in
 *                           little-endian order; the actual message starts
 *                           from the 4th byte
 *  0x03     message continuation - payload contains message fragment
 *  0x04     reset protocol - interrupt any message reception and sending
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

/* FIXME: Temporary ugly code! */
void RAWHID_PROTOCOL_task()
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

void RAWHID_PROTOCOL_handle_packet(uint8_t __attribute__((unused)) flags)
{
}
