#include "../util.h"
#include "../canny/canny.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <vector>

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

      if( canny[j*width+i] > 0 ){
	for(float theta = 0; theta < 360.f; theta += 360.f/thetaSize){

	  float r = float(i)*cos(theta * PI / 180.f)+float(j)*sin(theta * PI / 180.f);

	  if(r > 0.f){
	    int ri = r * float( rSize ) / maxR;
	    int thetai = theta * ( thetaSize / 360.f );
	    out[thetai*rSize + ri] += 1;
	  }
	}
      }

    }
  }
}

void findLines(
  int width,
  int height,
  int rSize,
  int thetaSize,
  float * hough,
  std::vector<int>& rList,
  std::vector<int>& thetaList){

  float maxR = sqrt(width*width+height*height);

  for( int r = 1; r < rSize-1; r++ ){
    for( int theta = 1; theta < thetaSize-1; theta++ ){
      float C = hough[(theta+0)*rSize+(r+0)];
      float N = hough[(theta-1)*rSize+(r+0)];
      float NE = hough[(theta-1)*rSize+(r+1)];
      float E = hough[(theta+0)*rSize+(r+1)];
      float SE = hough[(theta+1)*rSize+(r+1)];
      float S = hough[(theta+1)*rSize+(r+0)];
      float SW = hough[(theta+1)*rSize+(r-1)];
      float W = hough[(theta+0)*rSize+(r-1)];
      float NW = hough[(theta-1)*rSize+(r-1)];

      C -= 5.f; // threshold (throw out stuff that's not clearly the highest value)

      if(C > N && C > NE && C > E && C > SE && C > S && C > SW && C > W && C > NW){
	rList.push_back(r * maxR / float(rSize) );
	thetaList.push_back(theta);
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
  const int rSize = 800;
  const int thetaSize = 1440;
  float *houghOut = new float[rSize * thetaSize];

  hough(
    width,
    height,
    canny,
    rSize,
    thetaSize,
    houghOut);
  
  // find lines
  std::vector<int> rList;
  std::vector<int> thetaList;

  findLines( width, height, rSize, thetaSize, houghOut, rList, thetaList );

  for(unsigned int i=0; i<rList.size(); i++){
    printf("%d %d\n",rList[i],thetaList[i]);
  }

  // write out hough

  unsigned char *houghTemp = new unsigned char[rSize*thetaSize*3];

  const float houghOutScale = 1.f;

  for(int i=0; i<rSize*thetaSize; i++){
    houghTemp[i*3] = float(houghOut[i])*houghOutScale;
    houghTemp[i*3+1] = float(houghOut[i])*houghOutScale;
    houghTemp[i*3+2] = float(houghOut[i])*houghOutScale;
  }

  float maxR = sqrt(width*width+height*height);

  for(unsigned int i=0; i<rList.size(); i++){
    int j = thetaList[i]*rSize+(rList[i]*float(rSize)/maxR);
    houghTemp[j*3] = 255;
    houghTemp[j*3+1] = 0;
    houghTemp[j*3+2] = 0;
  }

  res = saveImage("hough.bmp", rSize, thetaSize, 3, houghTemp);

  // draw lines

  for(unsigned int i=0; i<rList.size(); i++){
    // find intercepts
    int x[2];
    int y[2];
    int intercepts = 0;

    float r = rList[i];
    float theta = thetaList[i]*PI/180.f;

    float ty = (-cos(theta)/sin(theta))*0+(r/sin(theta));
    if(ty >= 0 && ty < height){ x[intercepts] = 0; y[intercepts] = ty; intercepts++;}

    ty = (-cos(theta)/sin(theta))*(width-1)+(r/sin(theta));
    if(ty >= 0 && ty < height){ x[intercepts] = width-1; y[intercepts] = ty; intercepts++;}

    float tx = (-sin(theta)/cos(theta))*0+(r/cos(theta));
    if(tx >= 0 && tx < width){ x[intercepts] = tx; y[intercepts] = 0; intercepts++;}

    tx = (-sin(theta)/cos(theta))*(height-1)+(r/cos(theta));
    if(tx >= 0 && tx < width){ x[intercepts] = tx; y[intercepts] = height - 1; intercepts++;}

    if(intercepts == 2){
      drawLine( width, height, data, x[0], y[0], x[1], y[1] );
    }
  }

  res = saveImage("in.bmp", width, height, channels, data);

  delete[] houghOut;
  delete[] canny;

  return 0;
}
