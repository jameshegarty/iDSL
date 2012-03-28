#include "../../util.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include "../../convolution/convolution.h"
#include <time.h>

int main(int argc, char **argv){

  if(argc!=3){
    printf("Usage: convolution image.bmp block\n");
    return 1;
  }

  const int gaussianKernelWidth = 5;
  const int gaussianKernelRadius = (gaussianKernelWidth-1)/2;
  float *gaussianKernel = new float[gaussianKernelWidth*gaussianKernelWidth];

  buildGaussianKernel2D( 2, gaussianKernelRadius, gaussianKernel );

  //  float sobelX[9] = {-1.f,0.f,1.f,-2.f,0.f,2.f,-1.f,0.f,1.f};

  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  unsigned char *pixelsTemp = new unsigned char[width*height];
  unsigned char *outTemp = new unsigned char[width*height];
  unsigned char *out = new unsigned char[width*height*channels];

  int blockSize = atoi(argv[2]);
  int blockCountX = width/blockSize;
  if(blockCountX<1){blockCountX=1;}
  int blockCountY = height/blockSize;
  if(blockCountY<1){blockCountY=1;}
  printf("blockSize %d blockCountX %d blockCountY %d\n",blockSize, blockCountX, blockCountY);

  timespec startTime;
  timespec endTime;

  // touch the memory

  for(int i=0; i<width*height*channels; i++){
    out[i]=1;
  }


  clock_gettime(CLOCK_REALTIME,&startTime);

  for(int i=0; i<width*height*channels; i++){
    out[i]=data[i];
  }

  /*
  for(int c=0; c<channels; c++){
    
    for(int i=0; i<width*height; i++){
      pixelsTemp[i] = data[i*channels+c];
    }


    // blur
    for(int blockX=0; blockX < blockCountX; blockX++){
      for(int blockY=0; blockY < blockCountY; blockY++){
	
	for(int x=blockX*blockSize; x<((blockX==blockCountX-1)?width:(blockX+1)*blockSize); x++){
	  for(int y=blockY*blockSize; y<((blockY==blockCountY-1)?height:(blockY+1)*blockSize); y++){

	    assert(x < width);
	    assert(y < height);

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
      }
    }


      convolve2DClamped( 
      width, 
      height, 
      pixelsTemp, 
      gaussianKernelWidth,
      gaussianKernelWidth,
      gaussianKernel,
      outTemp );

    // sobel
    
    for(int x=0; x<width; x++){
      for(int y=0; y<height; y++){

	float accum = 0.f;

	for(int i=0; i<3; i++){
	  for(int j=0; j<3; j++){
	    int ix = clamp(x+i-1, 0, width-1);
	    int iy = clamp(y+j-1, 0, height-1);
	    
	    accum += sobelX[j*3+i] * float(outTemp[iy*width+ix]);
	    
	  }
	}

	pixelsTemp[y*width+x] = accum;
      }
    }


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
      //out[i*channels+c] = pixelsTemp[i];
    }
  }
  */
  clock_gettime(CLOCK_REALTIME,&endTime);
  timespec diff_serial;
  diff_serial.tv_sec=endTime.tv_sec - startTime.tv_sec;
  diff_serial.tv_nsec=endTime.tv_nsec - startTime.tv_nsec;
  std::cout << "Serial Code Runtime: " << ((double)diff_serial.tv_sec + ((double)diff_serial.tv_nsec/1E9)) << std::endl;
  double timeSec = ((double)diff_serial.tv_sec + ((double)diff_serial.tv_nsec/1E9));
  std::cout << timeSec << std::endl;
  std::cout << (double(width*height*channels)/timeSec) << std::endl;
  std::cout << (double(width*height*channels)/timeSec)/(1073741824.f) << std::endl;

  res = saveImage("out.bmp", width, height, channels, out);
  res = saveImage("in.bmp", width, height, channels, data);

  delete[] pixelsTemp;
  delete[] outTemp;
  delete[] out;

  return 0;
}
