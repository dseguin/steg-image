steg-image
==========
Steganographically embed data into the color information of an image. The data is stored in the 2 least significant bits of each color channel. This program makes use of the excellent [stb_image.h](https://github.com/nothings/stb) header library. Only PNG support is enabled, because JPG encoding is lossy and corrupts the embedded data.

### Example
```
$ ./steg-encode my-image.png my-data.bin
Successfully wrote to image file my-image.png.png
$ ./steg-decode my-image.png.png > output.bin
Embedded data is 192720 bytes
$ ls -1
my-data.bin
my-image.png
my-image.png.png
output.bin
steg-decode
steg-encode
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

### steg-encode
`USAGE: ./steg-encode IMAGE_FILE DATA_TO_EMBED`
steg-encode takes 2 arguments: an image file and a file to embed within the image. The data is broken up into 2-bit chunks that are stored in reverse order in to color channels of the image, replacing the 2 least significant bits of each channel. This ensures the embedded data does not significantly change the image visually. For this reason, you can only embed data up to `1/4 * number_of_pixels * number_of_channels` in bytes.

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

### steg-decode
`USAGE: ./steg-decode IMAGE_FILE`
steg-decode takes 1 argument: an image file that contains data embedded with `steg-encode`. The retrieved data is sent to `stdout`, so you will want to redirect it to a file or something. Again, this only works with PNG images.

### License
steg-image is released under the open source MIT license.
