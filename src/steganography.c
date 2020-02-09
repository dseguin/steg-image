#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define SET_ENCODE 0x01
#define SET_DECODE 0x02

/*image*/
unsigned char *i_data;
/*embedded data*/
unsigned char *e_data;
/*steg operation to perform*/
unsigned char op;
const char *e_file;
const char *i_file;
const char *prog_name;

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
	if(i_len < e_len * 4)
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

void print_usage(void)
{
	fprintf(stderr, "\nUSAGE: %s [-d|-e DATA_TO_EMBED] -i IMAGE_FILE\n\n", prog_name);
	fprintf(stderr, "  -i  Use IMAGE_FILE for steganography\n");
	fprintf(stderr, "  -d  Decode the data embedded in IMAGE_FILE\n");
	fprintf(stderr, "      and send the result to stdout\n");
	fprintf(stderr, "  -e  Embed the file DATA_TO_EMBED into IMAGE_FILE\n");
	fprintf(stderr, "      and send the result to stdout\n\n");
}

int process_args(int n, char **s)
{
	int i;
	for(i = 0; i < n; i++) {
		if(s[i][0] != '-')
			continue;
		switch(s[i][1]) {
			case 'e':
				if(i == n-1) {
					print_usage();
					return 1;
				}
				op = SET_ENCODE;
				e_file = s[i+1];
				break;
			case 'd':
				op = SET_DECODE;
				break;
			case 'i':
				if(i == n-1) {
					print_usage();
					return 1;
				}
				i_file = s[i+1];
				break;
			case 'h':
				print_usage();
				return 1;
			default:
				break;
		}
	}
	if(!op) {
		print_usage();
		return 1;
	}
	return 0;
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

int steg_encode(int x, int y, int n)
{
	unsigned e_len;
	unsigned char *png = NULL;
	int png_len = 0;
	fprintf(stderr, "Embedding \"%s\" into image data from \"%s\"...\n", e_file, i_file);
	if(!(e_len = load_data(e_file))) {
		fprintf(stderr, "%s: %d: load_data: Problem loading data from \"%s\" for embedding.\n", __FILE__, __LINE__ - 1, e_file);
		return 1;
	}
	embed_data(e_len, (unsigned)(x*y*n));
	png = stbi_write_png_to_mem(i_data, n*x, x, y, n, &png_len);
	if(!png) {
		fprintf(stderr, "%s: %d: stbi_write_png_to_mem: Problem converting to PNG image\n", __FILE__, __LINE__ - 2);
		return 1;
	}
	fprintf(stderr, "Writing resulting PNG image to stdout...\n");
	fwrite(png, png_len, 1, stdout);
	return 0;
}

int steg_decode(int x, int y, int n)
{
	unsigned e_len;
	fprintf(stderr, "Extracting embedded data from \"%s\"...\n", i_file);
	e_len = get_data((unsigned)(x*y*n));
	if(!e_len)
		return 1;
	fprintf(stderr, "Writing resulting data to stdout...\n");
	fwrite(e_data, e_len, 1, stdout);
	return 0;
}

int main(int argc, char **argv)
{
	int i_x, i_y, i_n;
	prog_name = argv[0];
	if(argc < 4 || argc > 5) {
		print_usage();
		return 1;
	}
	if(process_args(argc, argv))
		return 1;
	i_data = stbi_load(i_file, &i_x, &i_y, &i_n, 0);
	if(!i_data) {
		fprintf(stderr, "%s: %d: stbi_load: Problem loading image file \"%s\".\n", __FILE__, __LINE__ - 2, i_file);
		return 1;
	}
	if(op == SET_ENCODE)
		return steg_encode(i_x, i_y, i_n);
	else if(op == SET_DECODE)
		return steg_decode(i_x, i_y, i_n);
	return 0;
}
