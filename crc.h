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

/*! \file crc.h
 * Contains a function used to compute a CRC-16 fingerprint on an array
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

/*! Computes CRC-16
 * \param len the size of input data
 * \param data a pointer to the beginning of data
 * \return 2-byte CRC-16 value
 */
uint16_t crc16(size_t len, const uint8_t *data);
