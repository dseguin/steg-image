steg-image
==========
Steganographically embed data into the color information of an image. The data is stored in the least significant bit(s) of each color channel, which can be from 1 to 8. This program makes use of the excellent [stb_image.h](https://github.com/nothings/stb) header library. Only PNG support is enabled, because JPG encoding is lossy and corrupts the embedded data.

Compilation produces libsteg.so, which contains all the logic neccessary to encode and decode images. See [steganography.h](https://github.com/dseguin/steg-image/blob/master/src/libsteg/steganography.h) for a list of helper functions. You can include this file in your own project to take advantage of the libsteg.so library.

### Example
```
$ ./steg-run -i my_image.png -e my_data.bin > output.png
Using 2 least-significant bits (LSB) per color channel...
Embedding "my_data.bin" into image data from "my_image.png"...
Writing resulting PNG image to stdout...
$ ./steg-run -d -i output.png > output.bin
Using 2 least-significant bits (LSB) per color channel...
Extracting embedded data from "output.png"...
Embedded data is 198072 bytes
Writing resulting data to stdout...
$ ls -1
my_data.bin
my_image.png
output.bin
output.png
steg-image
steg-run
```

### Compiling
Things you will need:
- gcc
- GNU Make
- git
```
$ git clone https://github.com/dseguin/steg-image
$ cd steg-image
$ git submodule update --init
$ make
```

### Usage
```
USAGE: ./steg-run [-d|-e DATA_TO_EMBED] [-o OUT_FILE] [-b NUM_BITS] -i IMAGE_FILE

  -i  Use IMAGE_FILE for steganography
  -d  Decode the data embedded in IMAGE_FILE
      and send the result to stdout
  -e  Embed the file DATA_TO_EMBED into IMAGE_FILE
      and send the result to stdout
  -b  Use NUM_BITS as number of bits per color channel
      for embedded data (MIN: 1, MAX: 8, defaults to 2)
  -o  Write output to OUT_FILE (defaults to stdout)
```

`steg-run` is a convenience script that will load the libsteg.so library.

Using the `-e` flag, `steg-image` embeds the file `DATA_TO_EMBED` into the color data from `IMAGE_FILE`, and outputs the resulting PNG image to stdout (or `OUT_FILE` if using the -o flag). Using the `-d` flag, `steg-image` attempts to extract the data embedded in `IMAGE_FILE`, and outputs the resulting data to stdout (or `OUT_FILE` if using the -o flag). Again, this only works for PNG images currently.

The data is broken up into N-bit chunks (where 1 <= N <= 8) that are stored in reverse order in to color channels of the image, replacing the N least significant bits of each channel. This ensures the embedded data does not significantly change the image visually (if using a low number of bits). For this reason, you can only embed data up to `N/8 * number_of_pixels * number_of_channels` in bytes.

Assuming 2 bit chunks, the data is broken up like so:
```
embedded data:               01001011
                                 |
                                 v
                            01 00 10 11
                                 |
                                 v
                     11       10       00       01
image data:    11101011 10001110 01110100 00110101
```
The size of the embedded data is an unsigned int that is itself embedded at the start of the image data, using the first `CEIL(32/N)` bytes. Any form of image manipulation will corrupt the embedded data, so image hosting boards that perform image optimization will destroy anything you've embedded.

### License
steg-image is released under the open source MIT license.

steg-image makes use of [stb_image](https://github.com/nothings/stb) by Sean Barrett, which is also under the MIT license. License text for `stb_image` can be found [here](https://github.com/nothings/stb/raw/master/LICENSE).
