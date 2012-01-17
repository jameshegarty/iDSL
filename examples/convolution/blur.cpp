#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include "convolution.h"

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: convolution image.bmp\n");
    return 1;
  }

  const int gaussianKernelWidth = 5;
  float *gaussianKernel = new float[gaussianKernelWidth*gaussianKernelWidth];

  buildGaussianKernel2D(2,(gaussianKernelWidth-1)/2,gaussianKernel);

  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  unsigned char *pixelsTemp = new unsigned char[width*height];
  unsigned char *outTemp = new unsigned char[width*height];
  unsigned char *out = new unsigned char[width*height*channels];

  for(int c=0; c<channels; c++){
    
    for(int i=0; i<width*height; i++){
      pixelsTemp[i] = data[i*channels+c];
    }

    convolve2DClamped( 
      width, 
      height, 
      pixelsTemp, 
      gaussianKernelWidth,
      gaussianKernelWidth,
      gaussianKernel,
      outTemp );

    for(int i=0; i<width*height; i++){
      out[i*channels+c] = outTemp[i];
    }
  }

  res = saveImage("out.bmp", width, height, channels, out);
  res = saveImage("in.bmp", width, height, channels, data);

  delete[] pixelsTemp;
  delete[] outTemp;
  delete[] out;

  return 0;
}
