#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv){

  if(argc!=4){
    printf("Usage: blur image.type vectors.flo out.type\n");
    return 1;
  }

  int width, height, channels;
  int vwidth, vheight, vchannels;
  unsigned char *data;
  float *vdata;

  loadImage(argv[1], &width, &height, &channels, &data);
  loadImage(argv[2], &vwidth, &vheight, &vchannels, &vdata);

  assert(channels == 3);
  assert(vchannels == 2);
  assert(width==vwidth);
  assert(height==vheight);

  unsigned char *out = new unsigned char[width*height*channels];

  for(int x=0; x < width; x++){
    for(int y=0; y < height; y++){
      float xoff = vdata[2*(y*width+x)];
      float yoff = vdata[2*(y*width+x)+1];
      
      /*if(x+xoff >= 0 && x+xoff < width && y+yoff >= 0 && y+yoff < height){
	out[3*(y*width+x)+0] = data[3*((y+yoff)*width+x+xoff)+0];
	out[3*(y*width+x)+1] = data[3*((y+yoff)*width+x+xoff)+1];
	out[3*(y*width+x)+2] = data[3*((y+yoff)*width+x+xoff)+2];
	}*/
      sampleBilinear3Channels(width,height,float(x)+xoff,float(y)+yoff,data,&out[3*(y*width+x)]);
    }
  }

  saveImage(argv[3], width, height, channels, out);

  return 0;
}
