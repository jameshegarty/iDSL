#include "../util.h"
#include "canny.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "../convolution/convolution.h"

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: blur image.type");
    return 1;
  }

  // load image
  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);
  unsigned char *out = new unsigned char[width*height];

  cannyEdgeDetection( width, height, channels, data, out );

  res = saveImage("out.bmp", width, height, 1, out);
  res = saveImage("in.bmp", width, height, channels, data);

  delete[] data;
  delete[] out;

  return 0;
}
