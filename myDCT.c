//=======================================================
// Nicholai Venetucci (CS490) & Joseph Venetucci (CS590)
// Intro to Multimedia Net.
// PA2 myDCT
//
// Usage:
// 	$ ./myDCT input.pgm quantfile qScale output.dct
//=======================================================

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

void printIntBlock(int block[8][8]){
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      printf("%3d ", block[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printDoubleBlock(double block[8][8]){
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      printf("%7.2f ", block[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void dct(int block[8][8], double dctBlock[8][8]){
  double cu, cv, sum;

  for(int u = 0; u < 8; u++){
    for(int v = 0; v < 8; v++){
      if(u == 0)
        cu = 1.0 / sqrt(2.0);
      else
        cu = 1.0;
      if(v == 0)
        cv = 1.0 / sqrt(2.0);
      else
        cv = 1.0;

      sum = 0.0;
      for(int x = 0; x < 8; x++){
        for(int y = 0; y < 8; y++){
          sum += block[x][y] * 
		 cos(((2.0 * x + 1.0) * u * M_PI) / 16.0) *
		 cos(((2.0 * y + 1.0) * v * M_PI) / 16.0);
	}
      }

      dctBlock[u][v] = (cu / 2.0) * (cv / 2.0) * sum;
    }
  }
}

void quantization(double dctBlock[8][8], int qMatrix[8][8], float qScale, int qBlock[8][8]){
  int qCoeff;

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      qCoeff = roundf(dctBlock[x][y] / (qMatrix[x][y] * qScale));

      // crop to range [-127,128]
      if(qCoeff > 128)
        qCoeff = 128;
      else if(qCoeff < -127)
        qCoeff = -127;
      qCoeff += 127; // to range [0,255]

      qBlock[x][y] = qCoeff;
    }
  }
}

void zigZag(int qBlock[8][8], int zzBlock[8][8]){
  int x[] = {0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0,
	     1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3,
	     4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6,
	     7, 7, 6, 5, 4, 3, 4, 5, 6, 7, 7, 6, 5, 6, 7, 7};
  int y[] = {0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5,
	     4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4,
	     3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3,
	     2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 5, 6, 7, 7, 6, 7};

  // rearrange into zzBlock[8][8] (Zig-zag reordered block)
  int index = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      zzBlock[i][j] = qBlock[x[index]][y[index]];
      index++;
    }
  }
}

void writeOut(FILE * ofp, int zzBlock[8][8], int x, int y, int bm, int bn){
  fprintf(ofp, "%d %d\n", x+bn, y+bm); // write out x & y coordinate

  // write out the zig-zagged, clipped block
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      fprintf(ofp, "%5d", zzBlock[i][j]);
    }
    fprintf(ofp, "\n");
  }
}

int main(int argc, char * argv[]){
  if(argc != 5){
    printf("invalid number of command line arguments\n");
    printf("myDCT <input.pgm> <quantfile> <qscale> <output.dct>\n");
    exit(1);
  }

  FILE * ifp, * quantFile, * ofp;
  char magicNum[2]; // P5
  int width, height, maxGrayVal; // maxGrayVal = 255
  unsigned char macroBlock[16][16];
  int block[8][8];
  double dctBlock[8][8];
  int qMatrix[8][8];
  float qScale = atof(argv[3]);
  int qBlock[8][8];
  int zzBlock[8][8];

  ifp = fopen(argv[1], "rb");
  // read in PGM header
  fscanf(ifp, "%s", magicNum);
  fscanf(ifp, "%d", &width);
  fscanf(ifp, "%d", &height);
  fscanf(ifp, "%d", &maxGrayVal);
  fgetc(ifp);

  unsigned char pixels[width*height];
  // read bytes of image data into pixels[width*height]
  for(int i = 0; i < (width*height); i++)
    pixels[i] = fgetc(ifp);
  fclose(ifp);

  // read quantization file into qMatrix[8][8]
  quantFile = fopen(argv[2], "rb");
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      fscanf(quantFile, "%d", &qMatrix[i][j]);
  fclose(quantFile);

  ofp = fopen(argv[4], "wb");
  // print header for output file
  fprintf(ofp, "MYDCT\n");
  fprintf(ofp, "%d %d\n", width, height);
  fprintf(ofp, "%f\n", qScale);

  int index = 0;    // used to traverse pixels[width*height]
  int bm, bn;       // keep track of block position in a macro block (top left, top right, etc.)
  int x = 0, y = 0; // coordinates used for the output file

  for(int i = 0; i < ((width*height)/256); i++){ // for each macro block in the image
    for(int j = 0; j < 16; j++){
      for(int k = 0; k < 16; k++){
        macroBlock[j][k] = pixels[index++]; // populate our macroBlock[16][16]
      }

      // ensure macro blocks are processed left to right, top to bottom order
      if(width > 16){
        index += (width - 16); // increment index to compensate for scanline order
      }
    }

    for(int l = 0; l < 4; l++){ // for each block in a macro block
      if(l == 0)
        bm = 0, bn = 0; // top left
      else if(l == 1)
        bm = 0, bn = 8; // top right
      else if(l == 2)
        bm = 8, bn = 0; // bottom left
      else if(l == 3)
        bm = 8, bn = 8; // bottom right

      for(int m = 0; m < 8; m++){
        for(int n = 0; n < 8; n++){
          block[m][n] = macroBlock[m+bm][n+bn]; // populate our block[8][8]
	}
      }

      dct(block, dctBlock); // discrete cosine transform
      quantization(dctBlock, qMatrix, qScale, qBlock); // quantization
      zigZag(qBlock, zzBlock); // zig-zag reorder
      writeOut(ofp, zzBlock, x, y, bm, bn); // write to output file

      /** Uncomment to turn on Trace 
      printf("Spatial Block\n");
      printIntBlock(block); // print spatial block
      printf("DCT Block\n");
      printDoubleBlock(dctBlock);
      printf("Quantized & Cropped Block\n");
      printIntBlock(qBlock); // print quantized (and clipped) block
      printf("Zig Zag Reorded Block\n");
      printIntBlock(zzBlock); // print zig-zag reordered block (final block)
      **/
    }

    // ensure macro blocks are processed left to right, top to bottom order
    index -= (width - 16);
    if((index % (width * 16)) != 0){ // if there is still a macro block in the row
      index -= ((width * 16) - width); // adjust index to start at next macro block to the right
      x += 16;
    }
    else{ // don't adjust index, proceeding to next row of macro blocks
      x = 0;
      y += 16;
    }
  }

  fclose(ofp);
  return 0;
}
