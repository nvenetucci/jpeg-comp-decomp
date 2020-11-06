# Sample Files

Files to help verify the functionality of our *myDCT* and *myIDCT* programs.

* **.pgm** files are "uncompressed"
* **.dct** files are "compressed"
* **.dct.pgm** files are "decompressed"

### How to use?
Using *diff* to verify:
```bash
$ ./myDCT samples/test0.pgm samples/quantfile 1.0 output.dct

$ diff -s output.dct samples/test0.1.0.dct
Files output.dct and samples/test0.1.0.dct are identical
```

All together:
```bash
# Encode
$ ./myDCT samples/test2.pgm samples/quantfile 3.5 output.dct

$ diff -s output.dct samples/test2.3.5.dct
Files output.dct and samples/test2.3.5.dct are identical

# Decode
$ ./myIDCT output.dct samples/quantfile output.dct.pgm

$ diff -s output.dct.pgm samples/test2.3.5.dct.pgm
Files output.dct.pgm and samples/test2.3.5.dct.pgm are identical
```

### Bonus
Try opening both the uncompressed image (.pgm) and the decompressed image (.dct.pgm). Can you notice the loss in quality?