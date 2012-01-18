#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>

void buildGaussianKernel2D(float sigma, int kernelRadius, float *kernel){

  const int kWidth = kernelRadius*2+1;

  float a = 1.f/(2.f*3.14159f*sigma*sigma);

  for(int i=0; i<kWidth; i++){
    for(int j=0; j<kWidth; j++){
      float x = i-kernelRadius;
      float y = j-kernelRadius;
      kernel[j*kWidth+i] = a*exp(-(x*x+y*y)/(2.f*sigma*sigma));
    }
  }

  // normalize
  float sum = 0.f;
  for(int i=0; i<kWidth*kWidth; i++){sum += kernel[i];}
  sum = 1.f / sum;
  for(int i=0; i<kWidth*kWidth; i++){kernel[i] *= sum;}

  // debug
  //for(int i=0; i<kWidth*kWidth; i++){printf("%f\n",kernel[i]);}
}

// out of bounds values are clamped to the edge value
template <class T>
void convolve2DClampedInternal(
  int inWidth, 
  int inHeight, 
  T* in, 
  int kernelWidth,
  int kernelHeight,
  float *kernel,
  T* out){

  assert(kernelWidth % 2 == 1);
  assert(kernelHeight % 2 == 1);

  int kernelRadius = (kernelWidth-1)/2;

  for(int x=0; x<inWidth; x++){
    for(int y=0; y<inHeight; y++){

      float accum = 0.f;

      for(int i=0; i<kernelWidth; i++){
	for(int j=0; j<kernelHeight; j++){
	  int ix = clamp(x+i-kernelRadius, 0, inWidth-1);
	  int iy = clamp(y+j-kernelRadius, 0, inHeight-1);
	  
	  accum += kernel[j*kernelWidth+i] * float(in[iy*inWidth+ix]);
	}
      }

      out[y*inWidth+x] = accum;
    }
  }
}

void convolve2DClamped(
  int inWidth, 
  int inHeight, 
  unsigned char* in, 
  int kernelWidth,
  int kernelHeight,
  float *kernel,
  unsigned char* out){

  convolve2DClampedInternal(
    inWidth, 
    inHeight, 
    in, 
    kernelWidth,
    kernelHeight,
    kernel,
    out);
}

void convolve2DClamped(
  int inWidth, 
  int inHeight, 
  float* in, 
  int kernelWidth,
  int kernelHeight,
  float *kernel,
  float* out){

  convolve2DClampedInternal(
    inWidth, 
    inHeight, 
    in, 
    kernelWidth,
    kernelHeight,
    kernel,
    out);
}

// calculates out = a * b = b * a
// out of bounds values are 0
void convolve2D(
  int aWidth, 
  int aHeight, 
  float* a, 
  int bWidth, 
  int bHeight, 
  float* b, 
  int outWidth, 
  int outHeight, 
  float* out){

  assert(aWidth % 2 == 1);
  assert(aHeight % 2 == 1);
  assert(bWidth % 2 == 1);
  assert(bHeight % 2 == 1);
  assert(outWidth == std::max(aWidth+(bWidth-1),(aWidth-1)+bWidth));
  assert(outHeight == std::max(aHeight+(bHeight-1),(aHeight-1)+bHeight));

  int bOffsetX = (bWidth - aWidth)/2;
  bOffsetX = bOffsetX > 0 ? 0 : bOffsetX;
  int bOffsetY = (bWidth - aWidth)/2;
  bOffsetY = bOffsetY > 0 ? 0 : bOffsetY;

  for(int x=0; x<outWidth; x++){
    for(int y=0; y<outHeight; y++){
      float accum = 0.f;
      for(int i=0; i<aWidth; i++){
	for(int j=0; j<aHeight; j++){
	  int bx = bOffsetX + x + i - (aWidth-1)/2;
	  int by = bOffsetY + y + j - (aHeight-1)/2;
	  
	  float bValue = 0.f;
	  if(bx >= 0 && by >= 0 && bx < bWidth && by < bHeight){
	    bValue = float(b[by*bWidth+bx]);
	  }

	  accum += a[j*aWidth+i] * bValue;
	}
      }

      out[y*outWidth+x] = accum;
    }
  }
}
