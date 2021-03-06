#include <stdio.h>
#include <stdlib.h>
#include "libsteg/steganography.h"

#define SET_ENCODE 0x01
#define SET_DECODE 0x02

/*steg operation to perform*/
unsigned char op;
const char *e_file = NULL;
const char *i_file = NULL;
const char *o_file = NULL;
unsigned num_bits = 0;

void print_usage(const char *prog_name)
{
	fprintf(stderr, "\nUSAGE: %s [-d|-e DATA_TO_EMBED] [-o OUT_FILE] [-b NUM_BITS] -i IMAGE_FILE\n\n", prog_name);
	fprintf(stderr, "  -i  Use IMAGE_FILE for steganography\n");
	fprintf(stderr, "  -d  Decode the data embedded in IMAGE_FILE\n");
	fprintf(stderr, "      and send the result to stdout\n");
	fprintf(stderr, "  -e  Embed the file DATA_TO_EMBED into IMAGE_FILE\n");
	fprintf(stderr, "      and send the result to stdout\n");
	fprintf(stderr, "  -b  Use NUM_BITS as number of bits per color channel\n");
	fprintf(stderr, "      for embedded data (MIN: 1, MAX: 8, defaults to 2)\n");
	fprintf(stderr, "  -o  Write output to OUT_FILE (defaults to stdout)\n\n");
}

int process_args(int n, char **s)
{
	int i;
	for(i = 1; i < n; i++) {
		if(s[i][0] != '-')
			continue;
		switch(s[i][1]) {
		case 'e':
			if(!(e_file = (i == n-1) ? NULL : s[i+1]))
				return 1;
			op = SET_ENCODE;
			break;
		case 'd':
			op = SET_DECODE;
			break;
		case 'i':
			if(!(i_file = (i == n-1) ? NULL : s[i+1]))
				return 1;
			break;
		case 'o':
			if(!(o_file = (i == n-1) ? NULL : s[i+1]))
				return 1;
			break;
		case 'b': {
			char *c = NULL;
			if(i == n-1)
				return 1;
			num_bits = (unsigned)strtoul(s[i+1], &c, 10);
			if(num_bits < 1 || num_bits > 8 || (c && *c != '\0'))
				return 1;
			}
			break;
		case 'h':
			return 1;
		default:
			break;
		}
	}
	return (!op) ? 1 : 0;
}

int main(int argc, char **argv)
{
	int ret = 1;
	if(argc < 4 || process_args(argc, argv)) {
		print_usage(argv[0]);
		return ret;
	}
	steg_init();
	if(num_bits)
		set_num_bits(num_bits);
	if(load_image(i_file))
		return ret;
	if(op == SET_ENCODE)
		ret = steg_encode_to_file(e_file, o_file);
	else if(op == SET_DECODE)
		ret = steg_decode_to_file(o_file);
	else
		print_usage(argv[0]);
	steg_quit();
	return ret;
}
