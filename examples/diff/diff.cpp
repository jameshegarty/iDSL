#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>


int main(int argc, char **argv){

  if(argc!=3){
    printf("Usage: diff image1.type image2.type\n");
    printf("returns the number of pixels that are different\n");
    return 1;
  }

  int width, height, channels;
  int width2, height2, channels2;
  unsigned char *data;
  unsigned char *data2;

  loadImage(argv[1], &width, &height, &channels, &data);
  loadImage(argv[2], &width2, &height2, &channels2, &data2);

  assert(width==width2);
  assert(height==height2);
  assert(channels==channels2);

  int diffCount = 0;
  int total = 0;

  unsigned char *diffImage = new unsigned char[width*height];

  for(int i=0; i<width*height; i++){diffImage[i]=0;}

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){

      for(int c=0; c<channels; c++){
	if(data[(y*width+x)*3+c]!=data2[(y*width+x)*3+c]){
	  diffCount++;
	  diffImage[y*width+x]=255;
	  break;
	}
      }

      total++;
    }
  }

  saveImage("diff.bmp",width,height,1,diffImage);
  delete[] diffImage;

  //  printf("%d total:%d",diffCount,total);
  printf("%d\n",diffCount);

  return 0;
}
