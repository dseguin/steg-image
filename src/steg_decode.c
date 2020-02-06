#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "../ext/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../ext/stb/stb_image_write.h"

/*image*/
unsigned char *i_data;
/*embedded data*/
unsigned char *e_data;

unsigned get_data(unsigned i_len)
{
	unsigned i, j;
	unsigned e_len = 0;
	for(j = 0; j < 16; j++)
		e_len = e_len | ((unsigned)(i_data[j] & 0x03) << j*2);
	if(e_len > i_len/4) {
		fprintf(stderr, "ERROR: Embedded data is larger than image capacity (%u > %u/4). Either size of embedded data is corrupted or there is no embedded data in image.\n", e_len, i_len);
		return 0;
	}
	fprintf(stderr, "Embedded data is %u bytes\n", e_len);
	e_data = malloc(e_len);
	for(i = 16; i*4 < i_len && i-16 < e_len; i++) {
		unsigned char sect = 0;
		for(j = 0; j < 4; j++)
			sect |= ((i_data[i*4+j] & 0x03) << j*2);
		e_data[i-16] = sect;
	}
	return e_len;
}

int main(int argc, char ** argv)
{
	int i_x, i_y, i_n;
	unsigned e_len;
	if(argc < 2) {
		fprintf(stderr, "USAGE: %s IMAGE_FILE\n", argv[0]);
		return 1;
	}
	i_data = stbi_load(argv[1], &i_x, &i_y, &i_n, 0);
	if(!i_data) {
		fprintf(stderr, "%s: %d: stbi_load: Problem loading image file.\n", __FILE__, __LINE__ - 2);
		return 1;
	}
	e_len = get_data((unsigned)(i_x*i_y*i_n));
	if(!e_len)
		return 1;
	fwrite(e_data, e_len, 1, stdout);
	return 0;
}
