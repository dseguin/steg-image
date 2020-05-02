#ifndef _STEGANOGRAPHY_H
#define _STEGANOGRAPHY_H

#include <stdio.h>

/* image */
struct steg_image {
	int w;
	int h;
	int comps;
	unsigned size;
	const char *filename;
	FILE *fd;	/* used if destination is a stream */
	unsigned char *data;
};

/* embedded data */
struct steg_embed {
	unsigned size;
	const char *filename;
	FILE *fd;	/* used if destination is a stream */
	unsigned char *data;
};

/* call this first */
void steg_init(void);
/* call this last */
void steg_quit(void);
/* loads image into internal memory */
int load_image(const char *image_file);
/* writes result to the struct's fd (default is stdout) */
int steg_encode(const char *data_file);
int steg_decode(void);
/* writes result to file, or the struct's fd if "image_file" is NULL */
int steg_encode_to_file(const char *data_file, const char *image_file);
/* writes result to file, or the struct's fd if "data_file" is NULL */
int steg_decode_to_file(const char *data_file);

struct steg_image get_image(void);
struct steg_embed get_embedded(void);
/* passing NULL will reinitialize struct to zero */
void set_image(struct steg_image *image);
void set_embedded(struct steg_embed *embed);

#endif /*_STEGANOGRAPHY_H*/
