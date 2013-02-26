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
