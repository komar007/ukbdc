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

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define SCAN_OK_UNI	0
#define SCAN_OK_BI	1
#define SCAN_ETOOMANY	2
#define SCAN_ENONE	3

struct scan_result {
	uint8_t status;
	uint8_t a, b; /* pin pair */
};

struct scan_result SCANNER_scan(uint8_t noutputs, uint8_t outputs[],
		uint8_t ninputs, uint8_t inputs[]);
