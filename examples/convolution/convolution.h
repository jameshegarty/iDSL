#ifndef CONVOLUTION_H
#define CONVOLUTION_H 1

void buildGaussianKernel2D(float sigma, int kernelRadius, float *kernel);

void convolve2DClamped(
  int inWidth, 
  int inHeight, 
  unsigned char* in, 
  int kernelWidth,
  int kernelHeight,
  float *kernel,
  unsigned char* out);

void convolve2D(
  int aWidth, 
  int aHeight, 
  float* a, 
  int bWidth, 
  int bHeight, 
  float* b, 
  int outWidth, 
  int outHeight, 
  float* out);

#endif
