#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Implementation based on https://github.com/ogamespec/gc-ipl/blob/wiki/Descrambler.md
void descramble(uint8_t *data, int len)
{
	uint8_t acc = 0, nacc = 0, x = 1;
	uint16_t t = 0x2953, u = 0xd9c2, v = 0x3ff1;

	for (int it = 0; it < len;)
	{
		int t0 = t & 1,
			t1 = (t >> 1) & 1,
			u0 = u & 1,
			u1 = (u >> 1) & 1,
			v0 = v & 1;

		x ^= t1 ^ v0;
		x ^= u0 | u1;
		x ^= (t0 ^ u1 ^ v0) & (t0 ^ u0);

		if (t0 == u0)
		{
			v >>= 1;
			if (v0)
				v ^= 0xb3d0;
		}

		if (!t0)
		{
			u >>= 1;
			if (u0)
				u ^= 0xfb10;
		}

		t >>= 1;

		if (t0)
			t ^= 0xa740;

		acc = (acc << 1) + x;

		if (++nacc == 8)
		{
			data[it++] ^= acc;
			nacc = 0;
		}
	}
}

int main(int argc, char **argv)
{
	FILE *in = fopen(argv[1], "rb"),
		*out = fopen(argv[2], "wb");

	fseek(in, 0, SEEK_END);
	int size = ftell(in) - 0x100;
	rewind(in);

	uint8_t *hdr = malloc(0x100),
			*buf = malloc(size);

	fread(hdr, 1, 0x100, in);
	fread(buf, 1, size - 0x100, in);

	fclose(in);

	descramble(buf, size);

	fwrite(hdr, 1, 0x100, out);
	fwrite(buf, 1, size, out);

	free(hdr);
	free(buf);

	fclose(out);
}