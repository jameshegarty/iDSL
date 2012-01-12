#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

float *kernel;
int kernelRadius = 3;

void buildGaussianKernel(float sigma){
  kernel = new float[kernelRadius*2+1];

  float a = 1.f/(sqrt(2.f*3.14159f)*sigma);

  for(int i=0; i<(kernelRadius*2+1); i++){
    float x = i-kernelRadius;
    kernel[i] = a*exp(-(x*x)/(2.f*sigma*sigma));
  }

  // normalize
  float sum = 0.f;
  for(int i=0; i<(kernelRadius*2+1); i++){sum += kernel[i];}
  sum = 1.f / sum;
  for(int i=0; i<(kernelRadius*2+1); i++){kernel[i] *= sum;}

  // debug
  for(int i=0; i<(kernelRadius*2+1); i++){printf("%f\n",kernel[i]);}
}

template<bool X>
void convolve1D(int width, int height, unsigned char* in, unsigned char* out){
  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){

      float accum = 0.f;

      for(int i=0; i<(kernelRadius*2+1); i++){
	int ix = x;
	int iy = y;

	if(X){
	  ix = clamp(x+i-kernelRadius, 0, width-1);
	}else{
	  iy = clamp(y+i-kernelRadius, 0, height-1);
	}

	accum += kernel[i] * float(in[iy*width+(ix)]);
      }

      out[y*width+x] = accum;
    }
  }
}

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: convolution_separable image.bmp\n");
    return 1;
  }

  buildGaussianKernel(2);

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

    convolve1D<true>( width, height, pixelsTemp, outTemp );
    convolve1D<false>( width, height, outTemp, pixelsTemp );

    for(int i=0; i<width*height; i++){
      out[i*channels+c] = pixelsTemp[i];
    }
  }

  res = saveImage("out.bmp", width, height, channels, out);
  res = saveImage("in.bmp", width, height, channels, data);

  delete[] pixelsTemp;
  delete[] outTemp;
  delete[] out;

  return 0;
}
