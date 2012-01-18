#include "../util.h"
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

      if(ang > -22.5f && ang < 22.5f){
	float gXE = gradientX[j*width+i+1];
	float gYE = gradientY[j*width+i+1];
	float gMagE = sqrt(gXE*gXE+gYE*gYE);

	float gXW = gradientX[j*width+i+1];
	float gYW = gradientY[j*width+i+1];
	float gMagW = sqrt(gXW*gXW+gYW*gYW);

	if(gradientMag > gMagE && gradientMag > gMagW){
	  out[j*width+i] = 255;
	}else{
	  out[j*width+i] = 0;
	}
      }else if(ang < -67.5f && ang > 67.5f){
	float gXN = gradientX[(j-1)*width+i];
	float gYN = gradientY[(j-1)*width+i];
	float gMagN = sqrt(gXN*gXN+gYN*gYN);

	float gXS = gradientX[j*width+i+1];
	float gYS = gradientY[j*width+i+1];
	float gMagS = sqrt(gXS*gXS+gYS*gYS);

	if(gradientMag > gMagN && gradientMag > gMagS){
	  out[j*width+i] = 255;
	}else{
	  out[j*width+i] = 0;
	}
      }

    }
  }
}

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

  //printf("%f\n",sum(5,5,gaussianKernel));
  //printf("%f\n",sum(3,3,sobelX));
  //printf("%f\n",sum(cannyKernelSize,cannyKernelSize,cannyKernelX));

  // load image
  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  assert(channels == 3);

  float *dataMagnitude = new float[width*height];
  float *gradientX = new float[width*height];
  float *gradientY = new float[width*height];
  unsigned char *out = new unsigned char[width*height];

  for(int i=0; i<width*height; i++){
    float r = data[i*3];
    float g = data[i*3+1];
    float b = data[i*3+2];
    dataMagnitude[i] = sqrt(r*r+g*g+b*b);
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

  res = saveImage("out.bmp", width, height, 1, out);
  res = saveImage("in.bmp", width, height, channels, data);

  delete[] dataMagnitude;
  delete[] gradientX;
  delete[] gradientY;
  delete[] out;

  return 0;
}
