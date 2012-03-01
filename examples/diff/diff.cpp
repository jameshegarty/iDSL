#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(int argc, char **argv){

  if(argc!=4){
    printf("Usage: diff [type] image1.type image2.type\n");
    printf("[type] is either --L2 for l2 norm between pixels\n");
    printf("       or --pixels for # of pixels different\n");
    printf("       or --total for the total # of pixels in the image\n");
    printf("returns the number of pixels that are different\n");
    return 1;
  }

  int width, height, channels;
  int width2, height2, channels2;
  unsigned char *data;
  unsigned char *data2;

  enum diffTypeType {L2, Pixels, Total} diffType;

  if(strcmp(argv[1],"--L2")==0){
    diffType = L2;
  }else if(strcmp(argv[1],"--total")==0){
    diffType = Total;
  }else{
    diffType = Pixels;
  }

  if(!loadImage(argv[2], &width, &height, &channels, &data)){
    printf("Error loading image\n");
    return 1;
  }

  if(!loadImage(argv[3], &width2, &height2, &channels2, &data2)){
    printf("Error loading image\n");
    return 1;
  }

  assert(width==width2);
  assert(height==height2);
  assert(channels==channels2);

  int diffCount = 0;
  double diffCountDouble = 0;
  int total = 0;

  unsigned char *diffImage = new unsigned char[width*height];

  for(int i=0; i<width*height; i++){diffImage[i]=0;}

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){

      float L2Temp = 0;

      for(int c=0; c<channels; c++){
	if( data[(y*width+x)*channels+c] != data2[(y*width+x)*channels+c] && diffType == Pixels){
	  diffCount++;
	  diffImage[y*width+x]=255;
	  break;
	}

	if(diffType == L2){
	  L2Temp += pow(data[(y*width+x)*channels+c]-data2[(y*width+x)*channels+c],2);
	}
      }

      if(diffType == L2){
	diffCountDouble += L2Temp;
	diffImage[y*width+x] = L2Temp;
      }

      total++;
    }
  }

  saveImage("diff.bmp",width,height,1,diffImage);
  delete[] diffImage;

  //  printf("%d total:%d",diffCount,total);

  if(diffType == Pixels){
    printf("%d\n",diffCount);
  }else if(diffType == Total){
    printf("%d\n",total);
  }else{
    printf("%f\n",diffCountDouble);
  }

  return 0;
}
