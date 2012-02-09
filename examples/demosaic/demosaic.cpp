#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

void demosaicStupid(
  int width, 
  int height, 
  int xoff,
  int yoff,
  unsigned short* in, 
  unsigned short* out){

  // red
  for(int x=2; x<width-2; x++){
    for(int y=2; y<height-2; y++){
      if((x+xoff)%2==1 && (y+yoff)%2==1){ // red is known
	out[0*width*height + y*width+x] = in[y*width+x];
      }else if((x+xoff)%2==0 && (y+yoff)%2==1){ 
	float a = in[y*width+x-1];
	float b = in[y*width+x+1];
	out[0*width*height + y*width+x] = (a+b)/2.f;
      }else if((x+xoff)%2==1 && (y+yoff)%2==0){ 
	float a = in[(y+1)*width+x];
	float b = in[(y-1)*width+x];
	out[0*width*height + y*width+x] = (a+b)/2.f;
      }else{
	float a = in[(y+1)*width+x+1];
	float b = in[(y-1)*width+x-1];
	float c = in[(y+1)*width+x-1];
	float d = in[(y-1)*width+x+1];
       	float t = (a+b+c+d)/4.f;
	out[0*width*height + y*width+x] = t;	
      }
    }
  }

  // green
  for(int x=2; x<width-2; x++){
    for(int y=2; y<height-2; y++){
      if((x+xoff)%2==1 && (y+yoff)%2==0 || (x+xoff)%2 == 0 && (y+yoff)%2 == 1){
	// green is known
	out[1*width*height + y*width+x] = in[y*width+x];
      }else{ // interpolate
	float a = in[y*width+x+1];
	float b = in[y*width+x-1];
	float c = in[(y+1)*width+x];
	float d = in[(y-1)*width+x];
       	float t = (a+b+c+d)/4.f;
	out[1*width*height + y*width+x] = t;
      }
    }
  }

  // blue
  for(int x=2; x<width-2; x++){
    for(int y=2; y<height-2; y++){
      if((x+xoff)%2==0 && (y+yoff)%2==0){ // blue is known
	out[2*width*height + y*width+x] = in[y*width+x];
      }else if((x+xoff)%2==1 && (y+yoff)%2==0){ 
	float a = in[y*width+x-1];
	float b = in[y*width+x+1];
	out[2*width*height + y*width+x] = (a+b)/2.f;
      }else if((x+xoff)%2==0 && (y+yoff)%2==1){ 
	float a = in[(y+1)*width+x];
	float b = in[(y-1)*width+x];
	out[2*width*height + y*width+x] = (a+b)/2.f;
      }else{
	float a = in[(y+1)*width+x+1];
	float b = in[(y-1)*width+x-1];
	float c = in[(y+1)*width+x-1];
	float d = in[(y-1)*width+x+1];
       	float t = (a+b+c+d)/4.f;
	out[2*width*height + y*width+x] = t;	
      }

    }
  }

}

void equalize(int width, int height, unsigned short* in, unsigned short* out){

  for(int i=0; i<width*height; i++){
    out[i] = in[i]/10;
  }

}

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: demosaic image.pgm");
    return 1;
  }

  int width, height;
  unsigned short *data;

  loadPGM(argv[1], &width, &height, &data);

  unsigned short m = mean(width*height,data);
  printf("mean %d\n",m);

  unsigned short *pixelsTemp = new unsigned short[width*height*3];
  unsigned char *out = new unsigned char[width*height*3];

  demosaicStupid(width,height,1,1,data,pixelsTemp);

  for(int c=0; c<3; c++){
    equalize(width,height,&(pixelsTemp[width*height*c]), &(pixelsTemp[width*height*c]) );
  }

  for(int c=0; c<3; c++){
    for(int i=0; i<width*height; i++){
      out[i*3+c] = pixelsTemp[width*height*c+i];
    }
  }

  saveImage("out.bmp", width, height, 3, out);

  delete[] pixelsTemp;
  delete[] out;

  return 0;
}
