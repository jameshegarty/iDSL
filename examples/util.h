#ifndef UTIL_H
#define UTIL_H 1

#include <math.h>
#include <assert.h>
#include <string.h>
#include "convolution/convolution.h"

enum Color {RED, GREEN, BLUE, ALPHA, LUM};

const float PI = 3.1415926f;

bool loadImage(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool loadImage(const char *file, int* width, int* height, int *channels, unsigned short **data);
bool saveImage(const char *file, int width, int height, int channels, unsigned char *data);
// input data should be [0,1]
bool saveImage(const char *file, int width, int height, int channels, float *data);

bool loadBMP(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool saveBMP(const char *file, int width, int height, int channels, unsigned char *data);
bool saveBMP(const char *file, int width, int height, int channels, float *data);
bool loadPGM(const char *file, int* width, int* height, unsigned short **data); // return 16 bit data
bool loadPPM(const char *file, int* width, int* height, int* channels, unsigned char **data);
bool loadTMP(const char *file, int* width, int* height, int* channels, unsigned short **data); // return 16 bit data

template<class T, int channels, int channelSelector>
T sampleBilinearLow(int width, int height, float x, float y, const T* in){
  if(x < 0.f){return 0;}
  if(y < 0.f){return 0;}
  if(x >= float(width-1)){return 0;}
  if(y >= float(height-1)){return 0;}

  int ix = x;
  int iy = y;

  float aa = in[(iy*width+ix)*channels+channelSelector];
  float ba = in[(iy*width+ix+1)*channels+channelSelector];
  float ab = in[((iy+1)*width+ix)*channels+channelSelector];
  float bb = in[((iy+1)*width+ix+1)*channels+channelSelector];
  float rx = x-floor(x);
  float rxi = 1.f-rx;
  float ry = y-floor(y);
  float ryi = 1.f-ry; // 1-ry
  assert(rx >= 0.f);assert(rx <=1.f);assert(rxi >=0.f);assert(rxi <= 1.f);
  float resa = aa*rxi+ba*rx;
  float resb = ab*rxi+bb*rx;

  return (resa*ryi+resb*ry);
}

template<class T>
T sampleBilinear(int width, int height, float x, float y, const T* in){
  return sampleBilinearLow<T,1,0>(width,height,x,y,in);
}

template<class T>
void sampleBilinear3Channels(int width, int height, float x, float y, const T* in, T* out){
  out[0] = sampleBilinearLow<T,3,0>(width,height,x,y,in);
  out[1] = sampleBilinearLow<T,3,1>(width,height,x,y,in);
  out[2] = sampleBilinearLow<T,3,2>(width,height,x,y,in);
}

unsigned char sampleNearest(int width, int height, float x, float y, const unsigned char* in);

void normalizeKernel(int width, int height, float *kernel);
float sum(int width, int height, float *kernel);

void drawLine( int width, int height, unsigned char *data, int x0, int y0, int x1, int y1);

void toGrayscale( int width, int height, unsigned char *dataIn, float *grayscaleOut);
void toGrayscale( int width, int height, unsigned char *dataIn, unsigned char *grayscaleOut);

unsigned short mean(int size, unsigned short *in);

template<typename T>
T clamp(T Value, T Min, T Max){
  return (Value < Min)? Min : (Value > Max)? Max : Value;
}

// only works on 1 channel data for now
template<typename T>
void buildPyramid(
  int width, 
  int height, 
  int desiredLevels,
  T *input,
  T ***output){

  assert( desiredLevels > 0 );
  assert( width % int(pow(2,desiredLevels-1)) == 0 );
  assert( height % int(pow(2,desiredLevels-1)) == 0 );

  (*output) = new T*[desiredLevels];
  (*output)[0] = input;

  T *temp = new T[width*height];
  T *res = new T[width*height];
  T *origRes = res;

  const int kernelRadius = 2;
  float kernel[(kernelRadius*2+1)*(kernelRadius*2+1)];
  buildGaussianKernel2D(1,2,kernel);

  // iteratively blur and downsample
  for(int l=1; l < desiredLevels; l++){

    // blur
    convolve2DClamped(
      width/pow(2,l-1),
      height/pow(2,l-1),
      (*output)[l-1],
      kernelRadius*2+1,
      kernelRadius*2+1,
      kernel,
      temp);

    saveImage("lol.bmp", width/pow(2,l-1), height/pow(2,l-1), 1, temp);

    // downsample 
    int tw = width/int(pow(2,l));
    int th = height/int(pow(2,l));
    for(int x=0; x < tw; x++){
      for(int y=0; y < th; y++){
	res[tw*y+x] = temp[tw*4*y+x*2];
      }
    }
    
    saveImage("lol2.bmp", tw, th, 1, res);

    (*output)[l]=res;
    res += width*height/int(pow(2,l)*pow(2,l));
    assert(res-origRes < width*height);
  }

  delete[] temp;
}

unsigned char readPixel(int imgWidth, int imgHeight, int nChannels, 
                       int x, int y, Color colorChannel,
                       const unsigned char *data);

void writePixel(int imgWidth, int imgHeight, int nChannels, 
                        int x, int y, Color colorChannel,
                        unsigned char *data, unsigned char color);

#endif
