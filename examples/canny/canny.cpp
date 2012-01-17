#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "../convolution/convolution.h"


int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: blur image.type");
    return 1;
  }

  // build kernel
  float *gaussianKernel = new float[5*5];
  buildGaussianKernel2D(2.f, 2, gaussianKernel);

  float sobelX[] = {-1.f/4.f, 0.f, 1.f/4.f,
		    -2.f/4.f, 0.f, 2.f/4.f,
		    -1.f/4.f ,0.f, 1.f/4.f};

  const int cannyKernelSize = 7;
  float *cannyKernelX = new float[cannyKernelSize*cannyKernelSize];

  convolve2D(
    5, 
    5, 
    gaussianKernel, 
    3, 
    3, 
    sobelX, 
    cannyKernelSize, 
    cannyKernelSize, 
    cannyKernelX);

  saveImage("gaus.bmp", 5, 5, 1, gaussianKernel);
  saveImage("sobel.bmp", 3, 3, 1, sobelX);
  saveImage("kernel.bmp", cannyKernelSize, cannyKernelSize, 1, cannyKernelX);

  // load image
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
      cannyKernelSize,
      cannyKernelSize,
      cannyKernelX,
      outTemp);

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
