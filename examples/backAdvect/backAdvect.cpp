#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv){

  if(argc!=3){
    printf("Usage: blur image.type vectors.type\n");
    return 1;
  }

  int width, height, channels;
  int vwidth, vheight, vchannels;
  unsigned char *data;
  unsigned char *vdata;

  loadImage(argv[1], &width, &height, &channels, &data);
  loadImage(argv[2], &vwidth, &vheight, &vchannels, &vdata);

  assert(channels == 3);
  assert(vchannels == 3);

  unsigned char *out = new unsigned char[width*height*channels];

  for(int x=0; x < width; x++){
    for(int y=0; y < height; y++){
      int xoff = vdata[3*(y*width+x)];
      xoff = (xoff-128)/10;

      int yoff = vdata[3*(y*width+x)+1];
      yoff = (yoff-128)/10;

      out[3*(y*width+x)+0] = data[3*((y+yoff)*width+x+xoff)+0];
      out[3*(y*width+x)+1] = data[3*((y+yoff)*width+x+xoff)+1];
      out[3*(y*width+x)+2] = data[3*((y+yoff)*width+x+xoff)+2];
    }
  }

  saveImage("out.bmp", width, height, channels, out);

  return 0;
}
