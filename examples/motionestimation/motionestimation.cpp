#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void ofBrute(
  int width, 
  int height, 
  int windowRadius, 
  int diffRadius, 
  unsigned char* frame1, 
  unsigned char* frame2, 
  float* out){

  int border = windowRadius+diffRadius;

  printf("\n");

  for(int x = border; x < width - border; x++){
    printf("\b\b\b\b\b\b\b\b\b\b%03d / %03d\n",x,width-border);

    for(int y = border; y < height - border; y++){


      unsigned int lowestDiff = 1000000;
      int lowestDiffX=-100;
      int lowestDiffY=-100;

      // loop over search window
      for( int wx = -windowRadius; wx <= windowRadius; wx++){
	for( int wy = -windowRadius; wy <= windowRadius; wy++){

	  if(wx==0 && wy==0){
	    continue;
	  }

	  unsigned int ssd = 0;

	  // calculate SSD
	  for( int dx = -diffRadius; dx <= diffRadius; dx++){
	    for( int dy = -diffRadius; dy <= diffRadius; dy++){
	      int frame1Value = frame1[(y+dy)*width+(x+dx)];
	      int frame2Value = frame2[(y+wy+dy)*width+(x+wx+dx)];
	      ssd += pow(frame1Value-frame2Value,2);
	    }
	  }

	  if(ssd < lowestDiff){
	    lowestDiff = ssd;
	    lowestDiffX = wx;
	    lowestDiffY = wy;
	  }

	}
      }

      out[2*(y*width+x)]=lowestDiffX;
      out[2*(y*width+x)+1]=lowestDiffY;
    }
  }

}

int main(int argc, char **argv){

  if(argc!=6){
    printf("Usage: motionestimation frame1.type frame2.type output.flo searchWindowRadius diffWindowRadius\n");
    return 1;
  }

  int width, height, channels;
  int width2, height2, channels2;
  unsigned char *data;
  unsigned char *data2;

  if(!loadImage(argv[1], &width, &height, &channels, &data)){
    printf("Error loading image\n");
    return 1;
  }

  if(!loadImage(argv[2], &width2, &height2, &channels2, &data2)){
    printf("Error loading image\n");
    return 1;
  }

  assert(width==width2);
  assert(height==height2);
  assert(channels==channels2);
  assert(channels==3);

  unsigned char *frame1 = new unsigned char[width*height];
  unsigned char *frame2 = new unsigned char[width*height];
  float *out = new float[width*height*2];

  toGrayscale(width,height,data,frame1);
  toGrayscale(width,height,data2,frame2);

  saveImage("frame1gray.bmp", width, height, 1, frame1);
  saveImage("frame2gray.bmp", width, height, 1, frame2);

  // zero out vectors array
  for(int i=0; i<width*height*2; i++){out[i]=0;}

  ofBrute(width,height,atoi(argv[4]),atoi(argv[5]),frame1,frame2,out);

  // hack: flo considers y the opposite of our internal format
  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){
      out[2*(y*width+x)+1]*=-1.f;
    }
  }

  saveImage(argv[3], width, height, 2, out);

  delete[] frame1;
  delete[] frame2;
  delete[] out;

  return 0;
}
