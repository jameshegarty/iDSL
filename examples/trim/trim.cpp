#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char **argv){

  if(argc!=7){
    printf("Trim pixels from the sides of the image\n");
    printf("usage: trim in.type left right top bottom out.type\n");
    return 1;
  }

  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  unsigned char *out = new unsigned char[width*height*channels];

  int left = atoi(argv[2]);
  int right = atoi(argv[3]);
  int top = atoi(argv[4]);
  int bottom = atoi(argv[5]);

  int outWidth = width-left-right;
  int outHeight = height-top-bottom;

  for(int x=0; x<outWidth; x++){
    for(int y=0; y<outHeight; y++){
      for(int c=0; c<channels; c++){
	out[(y*outWidth+x)*channels+c] = data[((y+left)*width+x+left)*channels+c];
      }
    }
  }

  res = saveImage(argv[6], outWidth, outHeight, channels, out);

  delete[] out;

  return 0;
}
