#include "../util.h"
#include "../canny/canny.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

void hough( 
  int width, 
  int height, 
  unsigned char *canny, 
  int rSize,
  int thetaSize,
  float *out){

  float maxR = sqrt(width*width+height*height);

  for(int i=0; i<width; i++){
    for(int j=0; j<height; j++){

      if(canny[j*width+i]>0){
	for(int theta = 0; theta < 360; theta += 1){
	  float r = float(i)*cos(float(theta) * PI/180.f)+float(j)*sin(float(theta)*PI/180.f);

	  int ri = (r/maxR)*float(rSize);
	  out[theta*rSize + ri] += 1;
	}
      }

    }
  }
}

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: hough image.bmp");
    return 1;
  }

  // load image
  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  // compute canny
  unsigned char *canny = new unsigned char[width*height];
  cannyEdgeDetection( width, height, channels, data, canny );

  res = saveImage("canny.bmp", width, height, 1, canny);

  // compute hough
  const int rSize = 300;
  const int thetaSize = 360;
  float *houghOut = new float[rSize * thetaSize];

  hough(
    width,
    height,
    data,
    rSize,
    thetaSize,
    houghOut);

  for(int i=0; i<rSize*thetaSize; i++){
    houghOut[i]*=0.0001f;
  }

  res = saveImage("hough.bmp", rSize, thetaSize, 1, houghOut);

  // draw lines

  res = saveImage("in.bmp", width, height, channels, data);

  delete[] houghOut;
  delete[] canny;

  return 0;
}
