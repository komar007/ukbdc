#include "rawhid_protocol.h"
#include "platforms.h"
#include "layout.h"
#include "scanner.h"

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
#ifdef PLATFORM_alpha
#include "dataflash.h"
#endif

#include <util/delay.h>

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
			int old_n = n;
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
			/* switched off while layout support is being
			 * rewritten */
			//LAYOUT_set(ptr);
#ifdef PLATFORM_alpha
			DATAFLASH_write_page(1, sizeof(old_n), &old_n);
			_delay_ms(40);
			DATAFLASH_write_page(2, (uint16_t)old_n, ptr);
#endif
			break;
		} case MESSAGE_SCAN_MATRIX: {
			uint8_t timsk = TIMSK0;
			TIMSK0 = 0;
			struct scan_result result = SCANNER_scan(
					buf.payload[0], &buf.payload[1],
					buf.payload[1+buf.payload[0]], &buf.payload[2+buf.payload[0]]);
			char msg[64] = {0};
			msg[0] = result.status;
			msg[1] = result.a;
			msg[2] = result.b;
			RAWHID_send(&msg);
			TIMSK0 = timsk;
			break;
		} case MESSAGE_SET_MATRIX: {
			int n = buf.payload[0] + (buf.payload[1] << 8);
			int old_n = n;
			extern uint8_t matrix[19][8];
			uint8_t *ptr1 = matrix;
			while (n > 0) {
				while (!RAWHID_recv(&buf))
					;
				uint8_t nrecv = n > RAWHID_SIZE ? RAWHID_SIZE : n;
				memcpy(ptr1, &buf, nrecv);
				ptr1 += nrecv;
				n -= RAWHID_SIZE;
			}
#ifdef PLATFORM_alpha
			DATAFLASH_write_page(0, old_n, (uint8_t*)matrix);
#endif
			break;
		}
		}
	}
}

void RAWHID_PROTOCOL_handle_packet(uint8_t __attribute__((unused)) flags)
{
}
