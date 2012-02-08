#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

void buildDOG( int width, int height, float *data ){

}

void findExtrema(){

}

void calculateSiftFeatures( int width, int height, float *data ){
  buildDOG();
  findExtrema();
}

int main(int argc, char **argv){

  if(argc!=3){
    printf("Usage: sift imageA.bmp imageB.bmp");
    return 1;
  }

  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);


  res = saveImage("in.bmp", width, height, channels, data);

  delete[] data;

  return 0;
}
