#include "../../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include "../../convolution/convolution.h"

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: convolution image.bmp\n");
    return 1;
  }

  const int gaussianKernelWidth = 5;
  const int gaussianKernelRadius = (gaussianKernelWidth-1)/2;
  float *gaussianKernel = new float[gaussianKernelWidth*gaussianKernelWidth];

  buildGaussianKernel2D( 2, gaussianKernelRadius, gaussianKernel );

  float sobelX[9] = {-1.f,0.f,1.f,-2.f,0.f,2.f,-1.f,0.f,1.f};

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

    // blur
    for(int x=0; x<width; x++){
      for(int y=0; y<height; y++){

	float accum = 0.f;

	for(int i=0; i<gaussianKernelWidth; i++){
	  for(int j=0; j<gaussianKernelWidth; j++){
	    int ix = clamp(x+i-gaussianKernelRadius, 0, width-1);
	    int iy = clamp(y+j-gaussianKernelRadius, 0, height-1);
	    
	    accum += gaussianKernel[j*gaussianKernelWidth+i] * float(pixelsTemp[iy*width+ix]);
	    
	  }
	}

	outTemp[y*width+x] = accum;
      }
    }

    // sobel
    convolve2DClamped( 
      width, 
      height, 
      outTemp, 
      3,
      3,
      sobelX,
      pixelsTemp );

    // find max (3x3 kernel)
    for(int x=0; x<width; x++){
      for(int y=0; y<height; y++){

	unsigned char val = 0;

	for(int i=-1; i<=1; i++){
	  for(int j=-1; j<=1; j++){
	    int ix = clamp(x+i, 0, width-1);
	    int iy = clamp(y+j, 0, height-1);
	    
	    val = std::max( val, pixelsTemp[(iy*width)+ix]);
	  }
	}

	outTemp[(y*width)+x] = (pixelsTemp[(y*width)+x]==val)? 255 : 0;

      }
    }

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
