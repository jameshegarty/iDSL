#include "../util.h"
#include "canny.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "../convolution/convolution.h"


void nonMaximumSuppression(
  int width, 
  int height, 
  float *gradientX, 
  float *gradientY,
  unsigned char *out){

  for(int i=0; i<width; i++){
    for(int j=0; j<height; j++){
      float gX = gradientX[j*width+i];
      float gY = gradientY[j*width+i];
      float gradientMag = sqrt(gX*gX+gY*gY);
      float ang = atan2(gY,gX) * (180/PI); // [-90,90]

      int xOff = 0;
      int yOff = 0;

      if( (ang > -22.5f && ang < 22.5f) || (ang > 157.5f || ang < -157.5f) ){ // E/W
	xOff = 1;
	yOff = 0;
      }else if( (ang < 112.5f && ang > 67.5f) || (ang < -67.5f && ang > -112.5f)){
	xOff = 0;
	yOff = 1;
      }else if( (ang > 22.5f && ang < 67.5f) || (ang > -157.5f && ang < -112.5f)){
	xOff = 1;
	yOff = 1;
      }else if( (ang > 112.5f && ang < 157.5f) || (ang > -67.5f && ang < -22.5f)){
	xOff = 1;
	yOff = -1;
      }

      float gXA = gradientX[(j+yOff)*width+i+xOff];
      float gYA = gradientY[(j+yOff)*width+i+xOff];
      float gMagA = sqrt(gXA*gXA+gYA*gYA);
      
      float gXB = gradientX[(j-yOff)*width+i-xOff];
      float gYB = gradientY[(j-yOff)*width+i-xOff];
      float gMagB = sqrt(gXB*gXB+gYB*gYB);
      
      if(gradientMag >= gMagA && gradientMag > gMagB && gradientMag > 0.05f){
	out[j*width+i] = 255;
      }else{
	out[j*width+i] = 0;
      }

    }
  }
}

void cannyEdgeDetection( 
  int width, 
  int height, 
  int channels, 
  unsigned char *data,
  unsigned char *out){

  // build kernel
  float *gaussianKernel = new float[5*5];
  buildGaussianKernel2D(2.f, 2, gaussianKernel);

  float sobelX[] = {-1.f/4.f, 0.f, 1.f/4.f,
		    -2.f/4.f, 0.f, 2.f/4.f,
		    -1.f/4.f ,0.f, 1.f/4.f};

  float sobelY[] = {-1.f/4.f, -2.f/4.f, -1.f/4.f,
		    0.f, 0.f, 0.f,
		    1.f/4.f , 2.f/4.f, 1.f/4.f};

  const int cannyKernelSize = 7;
  float *cannyKernelX = new float[cannyKernelSize*cannyKernelSize];
  float *cannyKernelY = new float[cannyKernelSize*cannyKernelSize];

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

  convolve2D(
    3, 
    3, 
    sobelY,
    5, 
    5, 
    gaussianKernel, 
    cannyKernelSize, 
    cannyKernelSize, 
    cannyKernelY);

  //  cannyKernelSize = 3;
  //  cannyKernelX = sobelX;
  //  cannyKernelY = sobelY;

  //printf("%f\n",sum(5,5,gaussianKernel));
  //printf("%f\n",sum(3,3,sobelX));
  //printf("%f\n",sum(cannyKernelSize,cannyKernelSize,cannyKernelX));


  assert(channels == 3);

  float *dataMagnitude = new float[width*height];
  float *gradientX = new float[width*height];
  float *gradientY = new float[width*height];


  for(int i=0; i<width*height; i++){
    float r = data[i*3];
    float g = data[i*3+1];
    float b = data[i*3+2];
    dataMagnitude[i] = sqrt(r*r+g*g+b*b) / sqrt(3*255*255);
  }

  convolve2DClamped(
    width, 
    height, 
    dataMagnitude, 
    cannyKernelSize,
    cannyKernelSize,
    cannyKernelX,
    gradientX);

  convolve2DClamped(
    width, 
    height, 
    dataMagnitude, 
    cannyKernelSize,
    cannyKernelSize,
    cannyKernelY,
    gradientY);
  
  saveImage("gradientX.bmp", width, height, 1, gradientX);
  saveImage("gradientY.bmp", width, height, 1, gradientY);

  nonMaximumSuppression(width,height,gradientX,gradientY,out);

  delete[] dataMagnitude;
  delete[] gradientX;
  delete[] gradientY;
}
