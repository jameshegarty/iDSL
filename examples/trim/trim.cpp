#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

template<typename T>
void trim(
  T* data,
  int left,
  int right,
  int top,
  int bottom,
  int width,
  int height, 
  int channels,
  char * outfilename){

  T *out = new T[width*height*channels];

  int outWidth = width-left-right;
  int outHeight = height-top-bottom;

  for(int x=0; x<outWidth; x++){
    for(int y=0; y<outHeight; y++){
      for(int c=0; c<channels; c++){
	out[(y*outWidth+x)*channels+c] = data[((y+left)*width+x+left)*channels+c];
      }
    }
  }

  saveImage(outfilename, outWidth, outHeight, channels, out);

  delete[] out;
}

int main(int argc, char **argv){

  if(argc!=7){
    printf("Trim pixels from the sides of the image\n");
    printf("usage: trim in.type left right top bottom out.type\n");
    return 1;
  }

  int width, height, channels;
  unsigned char *data;



  if(!checkFloatImage(argv[1])){
    bool res = loadImage(argv[1], &width, &height, &channels, &data);
    if(!res){
      printf("error reading image\n");
      return 1;
    }

    trim(
	 data,
	 atoi(argv[2]),
	 atoi(argv[3]),
	 atoi(argv[4]),
	 atoi(argv[5]),
	 width,
	 height,
	 channels,
	 argv[6]);
  }else{
    float *dataF;
    bool res = loadImage(argv[1], &width, &height, &channels, &dataF);
    if(!res){
      printf("error reading image\n");
      return 1;
    }

    trim(
	 dataF,
	 atoi(argv[2]),
	 atoi(argv[3]),
	 atoi(argv[4]),
	 atoi(argv[5]),
	 width,
	 height,
	 channels,
	 argv[6]);

  }

  return 0;
}
