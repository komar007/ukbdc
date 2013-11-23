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

#include "scanner.h"
#include "io.h"

struct scan_result SCANNER_scan(uint8_t noutputs, uint8_t outputs[],
		uint8_t ninputs, uint8_t inputs[])
{
	struct scan_result result = {.status = SCAN_ENONE};
	bool found = false;
	for (int i = 0; i < noutputs; ++i) {
		IO_config(outputs[i], OUTPUT);
		IO_set(outputs[i], true);
	}
	for (int j = 0; j < ninputs; ++j) {
		IO_config(inputs[j], INPUT);
		IO_set(inputs[j], true); /* pull-up */
	}
	for (int i = 0; i < noutputs; ++i) {
		IO_set(outputs[i], false);
		for (int j = 0; j < ninputs; ++j) {
			if (IO_get(inputs[j])) /* if not pulled down */
				continue;
			if (!found) {
				result.a = outputs[i];
				result.b = inputs[j];
				result.status = SCAN_OK_UNI;
				found = true;
			} else {
				if (result.a == inputs[j] && result.b == outputs[i])
					result.status = SCAN_OK_BI;
				else
					result.status = SCAN_ETOOMANY;
			}
		}
		IO_set(outputs[i], true);
	}
	return result;
}
