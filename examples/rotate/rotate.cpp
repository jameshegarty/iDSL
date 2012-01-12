#include "../util.h"
#include <math.h>
#include <stdio.h>

unsigned char sample(int width, int height, float x, float y, unsigned char* in){
  if(x < 0.f){return 0;}
  if(y < 0.f){return 0;}
  if(x > float(width)){return 0;}
  if(y > float(height)){return 0;}
  float aa = in[int(y)*width+int(x)];
  float ba = in[int(y)*width+int(x)+1];
  float ab = in[(int(y)+1)*width+int(x)];
  float bb = in[(int(y)+1)*width+int(x)+1];
  float rx = x-floor(x);
  float rxi = floor(x)-x; // 1-rx
  float ry = y-floor(y);
  float ryi = floor(y)-y; // 1-ry
  float resa = aa*rxi+ba*rx;
  float resb = ab*rxi+bb*rx;
  return (unsigned char)(resa*ryi+resb*ry);
}

void rotate(int width, int height, unsigned char* in, unsigned char* out){
  float degrees = 42;

  float rotMatrix11 = cos(degrees); float rotMatrix12 = -sin(degrees);
  float rotMatrix21 = sin(degrees); float rotMatrix22 = cos(degrees);

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){
      float sx = rotMatrix11*x + rotMatrix12*y;
      float sy = rotMatrix21*x + rotMatrix22*y;

      out[y*width+x] = sample( width, height, sx, sy, in );
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
