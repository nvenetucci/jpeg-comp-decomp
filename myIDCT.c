//=======================================================
// Nicholai Venetucci (CS490) & Joseph Venetucci (CS590)
// Intro to Multimedia Net.
// PA2 myIDCT
//
// Usage:
// 	$ ./myIDCT input.dct quantfile output.pgm
//=======================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void printIntBlock(int block[8][8]){
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      printf("%3d ", block[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printDoubleBlock(double block[8][8]){
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      printf("%6.1f ", block[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void invZigZag(int block[8][8], int invZZBlock[8][8]){
  int x[] = {0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0,
	     1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3,
	     4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1, 2, 3, 4, 5, 6,
	     7, 7, 6, 5, 4, 3, 4, 5, 6, 7, 7, 6, 5, 6, 7, 7};
  int y[] = {0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5,
	     4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 6, 5, 4,
	     3, 2, 1, 0, 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3,
	     2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 5, 6, 7, 7, 6, 7};

  int index = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      invZZBlock[x[index]][y[index]] = block[i][j];
      index++;
    }
  }
}

void invQuant(int invZZBlock[8][8], int qMatrix[8][8], float qScale, double invQBlock[8][8]){
  double invQCoeff;

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      invQCoeff = invZZBlock[x][y];
      invQCoeff -= 127.0; // reset offset of pixel range to [-127,128]
      invQCoeff = (invQCoeff * qMatrix[x][y] * qScale);
      invQBlock[x][y] = invQCoeff;
    }
  }
}

void invDCT(double invQBlock[8][8], double invDCTBlock[8][8]){
  double cu, cv, sum;

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){

      sum = 0.0;
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

          sum += cu * cv * invQBlock[u][v] *
		 cos(((2.0 * x + 1.0) * u * M_PI) / 16.0) *
		 cos(((2.0 * y + 1.0) * v * M_PI) / 16.0);
	}
      }

      invDCTBlock[x][y] = (1.0 / 4.0) * sum;
    }
  }
}

void crop(double invDCTBlock[8][8], int clippedBlock[8][8]){
  int valToCrop;

  for(int x = 0; x < 8; x++){
    for(int y = 0; y < 8; y++){
      valToCrop = roundf(invDCTBlock[x][y]);

      // crop to range [0,255]
      if(valToCrop < 0)
        valToCrop = 0;
      else if(valToCrop > 255)
        valToCrop = 255;

      clippedBlock[x][y] = valToCrop;
    }
  }
}

int main(int argc, char * argv[]){
  if(argc != 4){
    printf("invalid number of command line arguments\n");
    printf("myIDCT <input.dct> <quantfile> <output.pgm>\n");
    exit(1);
  }

  FILE * ifp, * quantFile, * ofp;
  char magicNum[5]; // MYDCT
  int width, height, x, y;
  float qScale;
  int qMatrix[8][8];
  int block[8][8];
  int invZZBlock[8][8];
  double invQBlock[8][8];
  double invDCTBlock[8][8];
  int clippedBlock[8][8];

  // read in .dct header
  ifp = fopen(argv[1], "rb");
  fscanf(ifp, "%s", magicNum);
  fscanf(ifp, "%d", &width);
  fscanf(ifp, "%d", &height);
  fscanf(ifp, "%f", &qScale);

  unsigned char image[width][height]; // the entire image (to be remade)

  // read in quantization file
  quantFile = fopen(argv[2], "rb");
  for(int i = 0; i < 8; i++)
    for(int j = 0; j < 8; j++)
      fscanf(quantFile, "%d", &qMatrix[i][j]);
  fclose(quantFile);

  for(int i = 0; i < ((width*height)/64); i++){ // for each block in .dct file
    fscanf(ifp, "%d", &x); // get x & y coordinates
    fscanf(ifp, "%d", &y);

    for(int j = 0; j < 8; j++){
      for(int k = 0; k < 8; k++){
        fscanf(ifp, "%d", &block[j][k]); // populate our block[8][8]
      }
    }

    invZigZag(block, invZZBlock);
    invQuant(invZZBlock, qMatrix, qScale, invQBlock);
    invDCT(invQBlock, invDCTBlock);
    crop(invDCTBlock, clippedBlock);

    /** Uncomment to turn on Trace
    printf("%d %d\n", x, y);
    printf("Inverse Zig Zag Block\n");
    printIntBlock(invZZBlock); // print inverse zig-zag block
    printf("Inverse Quantized Block\n");
    printDoubleBlock(invQBlock); // print inverse quantization block
    printf("Inverse DCT Block\n");
    printDoubleBlock(invDCTBlock); // print inverse DCT block
    printf("Final Cropped Block\n");
    printIntBlock(clippedBlock); // print clipped block (rounded and cropped)
    **/

    // populate our image[width][height] with clippedBlock[8][8] using x & y coordinates
    int origx = x;
    for(int l = 0; l < 8; l++){
      for(int m = 0; m < 8; m++){
        image[y][x] = clippedBlock[l][m];
        x++;
      }
      x = origx;
      y++;
    }
  }

  // write to output file PGM header
  ofp = fopen(argv[3], "wb");
  fprintf(ofp, "P5\n");
  fprintf(ofp, "%d %d\n", width, height);
  fprintf(ofp, "255\n");
  // write out byte data of reconstructed image[width][height], scanline order
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      fprintf(ofp, "%c", image[i][j]);
    }
  }

  fclose(ofp);
  fclose(ifp);
  return 0;
}
