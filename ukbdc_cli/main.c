#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../firmware/scanner.h"

int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1], "r+");
	if (strcmp(argv[2], "dfu") == 0) {
		uint8_t buf[64] = {0};
		fwrite(buf, 64, 1, fp);
	} else if (strcmp(argv[2], "layout") == 0) {
		FILE *l = fopen(argv[3], "r");
		fseek(l, 0, SEEK_END);
		int size = ftell(l);
		fseek(l, 0, SEEK_SET);
		uint8_t buf[64] = {0x01};
		buf[1] = size & 0x00ff;
		buf[2] = size >> 8;
		fwrite(buf, 64, 1, fp);

		for (int n = size; n > 0; n -= 64) {
			fread(buf, n > 64 ? 64 : n, 1, l);
			fwrite(buf, 64, 1, fp);
		}
		fclose(l);
	} else if (strcmp(argv[2], "scan") == 0) {
start:
		printf("");
		uint8_t buf[64] = {
			0x02,	//scan
			8,	// noutputs
			0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, // outputs
			15,	// ninputs
			0, 1, 2, 3, 4, 5, 6, 7, 8, 13, 14, 15, 16, 17, 18 // inputs
		};
		fwrite(buf, 64, 1, fp);
		fread(buf, 64, 1, fp);
		int status = buf[0];
		if (status == SCAN_OK_UNI || status == SCAN_OK_BI)
			printf("%i %i\n", buf[1], buf[2]);
		else if (status == SCAN_ETOOMANY)
			printf("wrong\n");
		else
			goto start;
	} else if (strcmp(argv[2], "matrix") == 0) {
		FILE *l = fopen(argv[3], "r");
		fseek(l, 0, SEEK_END);
		int size = ftell(l);
		fseek(l, 0, SEEK_SET);
		uint8_t buf[64] = {0x03};
		buf[1] = size & 0x00ff;
		buf[2] = size >> 8;
		fwrite(buf, 64, 1, fp);

		for (int n = size; n > 0; n -= 64) {
			fread(buf, n > 64 ? 64 : n, 1, l);
			fwrite(buf, 64, 1, fp);
		}
		fclose(l);
	}
	fclose(fp);
	return 0;
}
