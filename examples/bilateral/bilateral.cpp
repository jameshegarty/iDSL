#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

float *kernel;
const int kernelRadius = 3;
const int kWidth = kernelRadius*2+1;
const float sigmaRange = 100.f;
const float sigmaSpatial = 2.f;

void buildGaussianKernel2D(float sigma){

  kernel = new float[kWidth*kWidth];

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

void convolve2D(int width, int height, unsigned char* in, unsigned char* out){
  float a = 1.f/(sqrt(2.f*3.14159f)*sigmaRange);

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){

      float accum = 0.f;
      float GrSum = 0.f;
      float Ip = in[y*width+x];

      for(int i=0; i<kWidth; i++){
	for(int j=0; j<kWidth; j++){
	  int ix = clamp(x+i-kernelRadius, 0, width-1);
	  int iy = clamp(y+j-kernelRadius, 0, height-1);
	  
	  float Iq = in[iy*width+ix];

	  float Gr = a*exp(-pow(abs(Ip-Iq),2)/(2.f*sigmaRange*sigmaRange));

	  GrSum += Gr*kernel[j*kWidth+i];
	  accum += kernel[j*kWidth+i] * Iq * Gr;
	}
      }
      float t=accum / GrSum;
      out[y*width+x] = t;
    }
  }
}

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: convolution image.bmp\n");
    return 1;
  }

  buildGaussianKernel2D(sigmaSpatial);

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

    convolve2D( width, height, pixelsTemp, outTemp );

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
