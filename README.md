# jpeg-comp-decomp

Implementation of a basic JPEG compression algorithm, as well as the decompression algorithm. Specifically, the compressor is implementing the DCT Transform and Quantization for a grayscale image. The decompressor serves to revert this process.

### Input File Format
The compression algorithm works with PGM files. PGM files can be generated using the *ppmtopgm* program. PPM files can be generated using either *giftopnm* or *djpeg* from the pbmplus libraries.

The format for PGM files are:
```
P5\n
<xsize> <ysize>\n
255\n
[xsize*ysize bytes of grayscale data, left to right, top to bottom]
```
For ease of implementation, this program only uses grayscale images.

### Compressed File Format
The compressed file takes the following format (referred to as .dct format):
```
MYDCT\n
<xsize> <ysize>\n
Qvalue\n
[xsize/16 * ysize/16 blocks of DCT coefficients]
```
Each "block" is encoded as:
```
x_offset(in pixels) y_offset(in pixels)
  [DCT values zig-zag reordered and written out as %5d]
```
*xsize* and *ysize* are multiples of 16 and are specified in ASCII characters. Input files are **only** multiples of 16. *Qvalue* is a floating point value specified in ASCII characters.

### Setup
Build with makefile:
```bash
$ make
```

### Compressor (Encoder) - *myDCT*
Usage:
```bash
$ ./myDCT <input image> <quantfile> <qscale> <output file>
```

### Decompressor (Decoder) - *myIDCT*
Usage:
```bash
$ ./myIDCT <input file> <quantfile> <output image>
```
