#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "steganography.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define DEFAULT_IMAGE_OUT stdout
#define DEFAULT_DATA_OUT stdout
/* Maximum of 8, minimum of 1 */
#define EMBED_BITS 2
#define CLAMP(x,low,high) ((x) < (low) ? (low) : ((x) > (high) ? (high) : (x)))

struct steg_image img = {0};
struct steg_embed e = {0};

struct steg_image get_image(void)
{
	return img;
}

struct steg_embed get_embedded(void)
{
	return e;
}

void set_image(struct steg_image *image)
{
	if(img.data)
		free(img.data);
	if(image)
		img = *image;
	else
		memset(&img, 0, sizeof(img));
}

void set_embedded(struct steg_embed *embed)
{
	if(e.data)
		free(e.data);
	if(embed)
		e = *embed;
	else
		memset(&e, 0, sizeof(e));
}

unsigned get_data_from_file(const char *filename)
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
	e.data = malloc(size + 1);
	rewind(f);
	fread(e.data, 1, size, f);
	fclose(f);
	e.filename = filename;
	e.size = size;
	return size;
}

unsigned _img_capacity(void)
{
	unsigned e_len = (sizeof(e.size)*8)/EMBED_BITS+CLAMP(8%EMBED_BITS,0,1);
	return (img.size*EMBED_BITS)/8 - e_len;
}

unsigned _embedded_meta_size(void)
{
	return (sizeof(e.size)*8)/EMBED_BITS + CLAMP(8%EMBED_BITS,0,1);
}

unsigned char _embedded_mask(void)
{
	unsigned i;
	unsigned char mask = 0x01;
	for(i = 1; i < EMBED_BITS; i++)
		mask = (mask << 1) | 0x01;
	return mask;
}

unsigned _extract_embedded_size(void)
{
	unsigned i, meta_bytes, size = 0;
	unsigned char mask;
	meta_bytes = _embedded_meta_size();
	mask = _embedded_mask();
	for(i = 0; i < meta_bytes; i++)
		size = size | ((unsigned)(img.data[i] & mask) << i*EMBED_BITS);
	if(size > _img_capacity()) {
		fprintf(stderr, "ERROR: Embedded data is larger than image capacity (%u > %u). Either size of embedded data is corrupted or there is no embedded data in image.\n", size, _img_capacity());
		return 0;
	}
	return size;
}

unsigned get_data_from_image(void)
{
	unsigned i, j, k = 0, l = 0, size = 0;
	l = _embedded_meta_size();
	size = _extract_embedded_size();
	if(!size)
		return 0;
	fprintf(stderr, "Embedded data is %u bytes\n", size);
	e.data = malloc(size);
	e.size = size;
	/* Lots of bugs in here */
	for(i = 0; i < e.size; i++) {
		unsigned char sect = 0;
		for(j = 0; j < 8; j++) {
			sect |= ((img.data[l] >> k) & 0x01) << j;
			if(++k == EMBED_BITS) {
				k = 0;
				l++;
			}
		}
		e.data[i] = sect;
	}
	return e.size;
}

unsigned _embed_meta_size(void)
{
	unsigned i, size, meta_bytes;
	unsigned char mask;
	meta_bytes = _embedded_meta_size();
	mask = _embedded_mask();
	size = e.size;
	if(e.size > _img_capacity()) {
		fprintf(stderr, "Data to embed is larger than image data. Truncating embedded data...\n");
		size = _img_capacity();
	}
	for(i = 0; i < meta_bytes; i++) {
		unsigned char sect;
		sect = (img.data[i] & (mask^0xff)) |
			(unsigned char)((size >> i*EMBED_BITS) & (unsigned)mask);
		img.data[i] = sect;
	}
	return size;
}

void embed_data(void)
{
	unsigned i, j, meta_bytes, size = 0, k = 0, l = 0;
	size = _embed_meta_size();
	meta_bytes = _embedded_meta_size();
	/* Lots of bugs in here */
	for(i = meta_bytes; l < size; i++) {
		unsigned char sect = 0;
		for(j = 0; j < EMBED_BITS; j++) {
			sect |= ((e.data[l] >> k) & 0x01) << j;
			if(++k == 8) {
				k = 0;
				l++;
			}
		}
		img.data[i] &= (_embedded_mask()^0xff);
		img.data[i] |= sect;
	}
}

int steg_encode(const char *e_file)
{
	unsigned char *png = NULL;
	int png_len = 0;
	fprintf(stderr, "Embedding \"%s\" into image data from \"%s\"...\n", e_file, img.filename);
	if(!get_data_from_file(e_file)) {
		fprintf(stderr, "%s: %d: get_data_from_file: Problem loading data from \"%s\" for embedding.\n", __FILE__, __LINE__ - 1, e_file);
		return 1;
	}
	e.filename = e_file;
	embed_data();
	png = stbi_write_png_to_mem(img.data, img.comps * img.w, img.w, img.h, img.comps, &png_len);
	if(!png) {
		fprintf(stderr, "%s: %d: stbi_write_png_to_mem: Problem converting to PNG image\n", __FILE__, __LINE__ - 2);
		return 1;
	}
	if(img.fd == DEFAULT_IMAGE_OUT)
		fprintf(stderr, "Writing resulting PNG image to stdout...\n");
	fwrite(png, png_len, 1, img.fd);
	return 0;
}

int steg_decode(void)
{
	fprintf(stderr, "Extracting embedded data from \"%s\"...\n", img.filename);
	get_data_from_image();
	if(!e.size)
		return 1;
	if(e.fd == DEFAULT_DATA_OUT)
		fprintf(stderr, "Writing resulting data to stdout...\n");
	fwrite(e.data, e.size, 1, e.fd);
	return 0;
}

/* This crime is necessary to consolidate the next two functions */
#define _ENCODE_DECODE_TO_FILE(struc,file,func) \
	do { \
		int ret = 0; \
		FILE *f, *f_old; \
		if(!file) \
			return func;\
		f_old = struc.fd; \
		f = fopen(file, "wb"); \
		if(!f) { \
			fprintf(stderr, "%s: %d: fopen: Could not open file for writing\n", __FILE__, __LINE__ - 2); \
			perror(file); \
			return 1; \
		} \
		struc.fd = f; \
		ret = func; \
		fprintf(stderr, "Writing result to %s...\n", file); \
		fclose(f); \
		struc.fd = f_old; \
		return ret; \
	} while(0)

int steg_encode_to_file(const char *e_file, const char *i_file)
{
	_ENCODE_DECODE_TO_FILE(img,i_file,steg_encode(e_file));
}

int steg_decode_to_file(const char *e_file)
{
	_ENCODE_DECODE_TO_FILE(e,e_file,steg_decode());
}

int load_image(const char *file)
{
	img.filename = file;
	img.data = stbi_load(file, &img.w, &img.h, &img.comps, 0);
	if(!img.data) {
		fprintf(stderr, "%s: %d: stbi_load: Problem loading image file \"%s\".\n", __FILE__, __LINE__ - 2, img.filename);
		return 1;
	}
	img.size = (unsigned)(img.w * img.h * img.comps);
	return 0;
}

void steg_init(void)
{
	img.fd = DEFAULT_IMAGE_OUT;
	e.fd = DEFAULT_DATA_OUT;
}

void steg_quit(void)
{
	if(img.data)
		free(img.data);
	if(e.data)
		free(e.data);
}
