#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

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
