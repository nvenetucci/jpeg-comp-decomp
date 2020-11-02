all: myDCT myIDCT

myDCT: myDCT.c
	gcc -o myDCT myDCT.c -lm

myIDCT: myIDCT.c
	gcc -o myIDCT myIDCT.c -lm
