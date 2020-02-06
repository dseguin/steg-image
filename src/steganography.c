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
		sect = (i_data[j] & 0xFC) | (unsigned char)((e_len >> j*2) & 0x00000003);
		i_data[j] = sect;
	}
	for(i = 16; i*4 < i_len && i-16 < e_len; i++) {
		for(j = 0; j < 4; j++) {
			unsigned char sect;
			sect = (e_data[i-16] >> j*2) & 0x03;
			i_data[i*4+j] &= 0xFC;
			i_data[i*4+j] |= sect;
		}
	}
}

int main(int argc, char ** argv)
{
	int i_x, i_y, i_n;
	int fname_len;
	unsigned e_len;
	char *new_file;
	if(argc < 3) {
		fprintf(stderr, "USAGE: %s IMAGE_FILE DATA_TO_EMBED\n", argv[0]);
		return 1;
	}
	i_data = stbi_load(argv[1], &i_x, &i_y, &i_n, 0);
	if(!i_data) {
		fprintf(stderr, "%s: %d: stbi_load: Problem loading image file.\n", __FILE__, __LINE__ - 2);
		return 1;
	}
	if(!(e_len = load_data(argv[2]))) {
		fprintf(stderr, "%s: %d: load_data: Problem loading data for embedding.\n", __FILE__, __LINE__ - 1);
		return 1;
	}
	embed_data(e_len, (unsigned)(i_x*i_y*i_n));
	fname_len = strlen(argv[1]);
	new_file = malloc(fname_len + 5);
	strcpy(new_file, argv[1]);
	new_file[fname_len] = '.';
	new_file[fname_len+1] = 'p';
	new_file[fname_len+2] = 'n';
	new_file[fname_len+3] = 'g';
	new_file[fname_len+4] = '\0';
	if(!stbi_write_png(new_file, i_x, i_y, i_n, i_data, i_n*i_x)) {
		fprintf(stderr, "%s: %d: stbi_write_jpg: Problem writing image file %s\n", __FILE__, __LINE__ - 1, new_file);
		return 1;
	}
	printf("Successfully wrote to image file %s\n", new_file);
	return 0;
}
