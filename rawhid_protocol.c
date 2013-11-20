#include "rawhid_protocol.h"
#include "atmel_bootloader.h"
#include "layout.h"
#include "crc.h"
#include "aux.h"
#include "main.h"

#include <avr/interrupt.h>
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
 *  0x01     pong - answer to the ping message;
 *                  the first byte of payload is device's status byte
 *  0x02     message start - the first packet of a message; the next byte
 *                           is the size of the message in bytes.
 *                           the two following bytes are the crc of the whole
 *                           message. The actual message starts from the 5th byte
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
 *  0x01 write layout page                       page number (1 byte), page
 *                                               data (128 bytes)
 *  0x02 activate layout                         none
 *  0x03 deactivate layout                       none
 *
 *  Device to Host:
 */

static volatile struct RAWHID_state state;

void RAWHID_PROTOCOL_task()
{
	if (state.status != EXECUTING)
		return;
	uint8_t hdr = state.msg[0];
	switch (hdr) {
	case MESSAGE_DFU:
		run_bootloader();
		break; /* well... */
	case MESSAGE_WRITE_PAGE:
		if (state.len != SPM_PAGESIZE + 2) {
			state.status = MESSAGE_ERROR;
			return;
		}
		const uint8_t pageno = state.msg[1];
		uint32_t addr = LAYOUT_BEGIN + pageno*SPM_PAGESIZE;
		flash_write_page(addr, (uint8_t*)state.msg + 2);
		break;
	case MESSAGE_ACTIVATE_LAYOUT:
		if (state.len != 1) {
			state.status = MESSAGE_ERROR;
			return;
		}
		LAYOUT_set((const struct layout*)LAYOUT_BEGIN);
		break;
	case MESSAGE_DEACTIVATE_LAYOUT:
		if (state.len != 1) {
			state.status = MESSAGE_ERROR;
			return;
		}
		LAYOUT_deactivate();
		break;
	default:
		state.status = WRONG_MESSAGE_ERROR;
		return;
	};
	state.status = IDLE;
}

void RAWHID_PROTOCOL_handle_packet(uint8_t __attribute__((unused)) flags)
{
	struct RAWHID_packet buf;
	if (!RAWHID_recv(&buf))
		return;
	switch (buf.header) {
	case MSG_START: {
		if (state.status == EXECUTING) {
			state.status = BUSY_ERROR;
			break;
		} else if (state.status != IDLE) {
			break;
		}
		state.len = buf.payload[0];
		state.crc = *(uint16_t*)&buf.payload[1];
		const int to_copy = min(RAWHID_SIZE - MSG_HDR_SIZE - 1, state.len);
		memcpy((uint8_t*)state.msg, buf.payload + MSG_HDR_SIZE, to_copy);
		state.recvd = to_copy;
		if (state.recvd >= state.len)
			goto check_crc;
		else
			state.status = RECEIVING_MESSAGE;
		break;
	} case MSG_CONT: {
		if (state.status != RECEIVING_MESSAGE) {
			state.status = UNEXPECTED_CONT_ERROR;
			break;
		}
		const int to_copy = min(RAWHID_SIZE - 1, state.len - state.recvd);
		memcpy((uint8_t*)state.msg + state.recvd, buf.payload, to_copy);
		state.recvd += RAWHID_SIZE - 1;
		if (state.recvd >= state.len) {
check_crc:
			if (crc16(state.len, (uint8_t*)state.msg) != state.crc) {
				state.status = CRC_ERROR;
				state.len = 0;
			} else {
				state.status = EXECUTING;
			}
		}
		break;
	} case PING: {
		buf.header = PONG;
		buf.payload[0] = state.status;
		RAWHID_send(&buf);
		break;
	} case RESET_PROTO:
		state.len = 0;
		state.recvd = 0;
		state.status = IDLE;
		break;
	}
}
