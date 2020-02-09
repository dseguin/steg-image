steg-image
==========
Steganographically embed data into the color information of an image. The data is stored in the 2 least significant bits of each color channel. This program makes use of the excellent [stb_image.h](https://github.com/nothings/stb) header library. Only PNG support is enabled, because JPG encoding is lossy and corrupts the embedded data.

### Example
```
$ ./steg-image -i my_image.png -e my_data.bin > output.png
Embedding "my_data.bin" into image data from "my_image.png"...
Writing resulting PNG image to stdout...
$ ./steg-image -d -i output.png > output.bin
Extracting embedded data from "output.png"...
Embedded data is 198072 bytes
Writing resulting data to stdout...
$ ls -1
my_data.bin
my_image.png
output.bin
output.png
steg-image
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
USAGE: ./steg-image [-d|-e DATA_TO_EMBED] -i IMAGE_FILE

  -i  Use IMAGE_FILE for steganography
  -d  Decode the data embedded in IMAGE_FILE
      and send the result to stdout
  -e  Embed the file DATA_TO_EMBED into IMAGE_FILE
      and send the result to stdout
```

Using the `-e` flag, `steg-image` embeds the file `DATA_TO_EMBED` into the color data from `IMAGE_FILE`, and outputs the resulting PNG image to stdout. Using the `-d` flag, `steg-image` attempts to extract the data embedded in `IMAGE_FILE`, and outputs the resulting data to stdout. In either case, you will want to redirect the output to a file or something. Again, this only works for PNG images currently.

The data is broken up into 2-bit chunks that are stored in reverse order in to color channels of the image, replacing the 2 least significant bits of each channel. This ensures the embedded data does not significantly change the image visually. For this reason, you can only embed data up to `1/4 * number_of_pixels * number_of_channels` in bytes.

The data is broken up like so:
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
The size of the embedded data is an unsigned int that is itself embedded at the start of the image data, using the first 16 bytes. Any form of image manipulation will corrupt the embedded data, so image hosting boards that perform image optimization will destroy anything you've embedded.

### License
steg-image is released under the open source MIT license.
