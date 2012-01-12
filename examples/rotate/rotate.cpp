#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

unsigned char sampleBilinear(int width, int height, float x, float y, unsigned char* in){
  if(x < 0.f){return 0;}
  if(y < 0.f){return 0;}
  if(x > float(width)){return 0;}
  if(y > float(height)){return 0;}

  int ix = x;
  int iy = y;

  float aa = in[iy*width+ix];
  float ba = in[iy*width+ix+1];
  float ab = in[(iy+1)*width+ix];
  float bb = in[(iy+1)*width+ix+1];
  float rx = x-floor(x);
  float rxi = 1.f-rx;
  float ry = y-floor(y);
  float ryi = 1.f-ry; // 1-ry
  assert(rx >= 0.f);assert(rx <=1.f);assert(rxi >=0.f);assert(rxi <= 1.f);
  float resa = aa*rxi+ba*rx;
  float resb = ab*rxi+bb*rx;

  return (unsigned char)(resa*ryi+resb*ry);
}

unsigned char sampleNearest(int width, int height, float x, float y, unsigned char* in){
  if(x < 0.f){return 0;}
  if(y < 0.f){return 0;}
  if(x > float(width)){return 0;}
  if(y > float(height)){return 0;}

  int ix = x;
  int iy = y;

  return in[iy*width+ix];
}

void rotate(int width, int height, unsigned char* in, unsigned char* out){
  float degrees = 0.20f;

  float rotMatrix11 = cos(degrees); float rotMatrix12 = -sin(degrees);
  float rotMatrix21 = sin(degrees); float rotMatrix22 = cos(degrees);

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){
      float sx = rotMatrix11*x + rotMatrix12*y;
      float sy = rotMatrix21*x + rotMatrix22*y;

      out[y*width+x] = sampleBilinear( width, height, sx, sy, in );
    }
  }
}

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: blur image.type");
    return 1;
  }

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

    rotate( width, height, pixelsTemp, outTemp );

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
