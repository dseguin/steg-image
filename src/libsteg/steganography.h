#ifndef _STEGANOGRAPHY_H
#define _STEGANOGRAPHY_H

int load_image(const char *file);
int steg_encode(const char *e_file);
int steg_decode(void);

#endif /*_STEGANOGRAPHY_H*/
