#include <stdio.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
	FILE *fp = fopen("/dev/hidraw1", "w");
	if (argc == 1) {
		uint8_t buf[64] = {0};
		fwrite(buf, 64, 1, fp);
	} else {
		FILE *l = fopen(argv[1], "r");
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
	}
	fclose(fp);
	return 0;
}
