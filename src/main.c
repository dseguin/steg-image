#include <stdio.h>
#include "libsteg/steganography.h"

#define SET_ENCODE 0x01
#define SET_DECODE 0x02

/*steg operation to perform*/
unsigned char op;
const char *e_file;
const char *i_file;

void print_usage(const char *prog_name)
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
	for(i = 1; i < n; i++) {
		if(s[i][0] != '-')
			continue;
		switch(s[i][1]) {
			case 'e':
				if(i == n-1) {
					print_usage(s[0]);
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
					print_usage(s[0]);
					return 1;
				}
				i_file = s[i+1];
				break;
			case 'h':
				print_usage(s[0]);
				return 1;
			default:
				break;
		}
	}
	if(!op) {
		print_usage(s[0]);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	if(argc < 4 || argc > 5) {
		print_usage(argv[0]);
		return 1;
	}
	if(process_args(argc, argv))
		return 1;
	if(load_image(i_file))
		return 1;
	if(op == SET_ENCODE)
		return steg_encode(e_file);
	else if(op == SET_DECODE)
		return steg_decode();
	return 0;
}