/* This file is part of ukbdc.
 *
 * ukbdc is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ukbdc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ukbdc; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*! \file system_messages.h
 * This file contains all types of messages used by the published-subscriber
 * messaging system.
 */

#pragma once

/*! Stores a message type */
typedef enum {
	/*! Used by the USB module, published on USB's start-of-frame */
	USB_SOF = 0,
	/*! Used by the TIMER module, published when a timer has finished
	 * counting time */
	TIMER,

	/*! The number of all message types */
	NUM_SYSTEM_MESSAGE_TYPES
} message_type_t;
