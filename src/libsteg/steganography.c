#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "steganography.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

/*image*/
unsigned char *i_data;
int i_x, i_y, i_n;
/*embedded data*/
unsigned char *e_data;
/*steg operation to perform*/
unsigned char op;
/*image filename*/
const char *i_file;

unsigned load_data(const char *filename)
{
	FILE *f = NULL;
	unsigned size = 0;
	f = fopen(filename, "rb");
	if(!f)
		return 0;
	if(fseek(f, 0L, SEEK_END)) {
		fclose(f);
		return 0;
	}
	size = (unsigned)ftell(f);
	if(size <= 0) {
		fclose(f);
		return 0;
	}
	e_data = malloc(size + 1);
	rewind(f);
	fread(e_data, 1, size, f);
	fclose(f);
	return size;
}

void embed_data(unsigned e_len, unsigned i_len)
{
	unsigned i, j;
	if(i_len < e_len * 4 + 64)
		fprintf(stderr, "Data to embed is larger than image data. Truncating embedded data...\n");
	for(j = 0; j < 16; j++) {
		unsigned char sect;
		sect = (i_data[j] & 0xFC) |
			(unsigned char)((e_len >> j*2) & 0x00000003);
		i_data[j] = sect;
	}
	/* Lots of bugs in here */
	for(i = 16; i*4 < i_len && i-16 < e_len; i++) {
		for(j = 0; j < 4; j++) {
			unsigned char sect;
			sect = (e_data[i-16] >> j*2) & 0x03;
			i_data[i*4+j] &= 0xFC;
			i_data[i*4+j] |= sect;
		}
	}
}

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
	/* Lots of bugs in here */
        for(i = 16; i*4 < i_len && i-16 < e_len; i++) {
                unsigned char sect = 0;
                for(j = 0; j < 4; j++)
                        sect |= ((i_data[i*4+j] & 0x03) << j*2);
                e_data[i-16] = sect;
        }
        return e_len;
}

int steg_encode(const char *e_file)
{
	unsigned e_len;
	unsigned char *png = NULL;
	int png_len = 0;
	fprintf(stderr, "Embedding \"%s\" into image data from \"%s\"...\n", e_file, i_file);
	if(!(e_len = load_data(e_file))) {
		fprintf(stderr, "%s: %d: load_data: Problem loading data from \"%s\" for embedding.\n", __FILE__, __LINE__ - 1, e_file);
		return 1;
	}
	embed_data(e_len, (unsigned)(i_x*i_y*i_n));
	png = stbi_write_png_to_mem(i_data, i_n*i_x, i_x, i_y, i_n, &png_len);
	if(!png) {
		fprintf(stderr, "%s: %d: stbi_write_png_to_mem: Problem converting to PNG image\n", __FILE__, __LINE__ - 2);
		return 1;
	}
	fprintf(stderr, "Writing resulting PNG image to stdout...\n");
	fwrite(png, png_len, 1, stdout);
	return 0;
}

int steg_decode(void)
{
	unsigned e_len;
	fprintf(stderr, "Extracting embedded data from \"%s\"...\n", i_file);
	e_len = get_data((unsigned)(i_x*i_y*i_n));
	if(!e_len)
		return 1;
	fprintf(stderr, "Writing resulting data to stdout...\n");
	fwrite(e_data, e_len, 1, stdout);
	return 0;
}

int load_image(const char *file)
{
	i_file = file;
	i_data = stbi_load(file, &i_x, &i_y, &i_n, 0);
	if(!i_data) {
		fprintf(stderr, "%s: %d: stbi_load: Problem loading image file \"%s\".\n", __FILE__, __LINE__ - 2, i_file);
		return 1;
	}
	return 0;
}
