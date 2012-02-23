#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "svdcmp.h"

// calculate the pseudo inverse in place
void pinv2by2(float *A){
  double **u = dmatrix(1,2,1,2);
  double **v = dmatrix(1,2,1,2);
  u[0][0]=A[0];
  u[0][1]=A[1];
  u[1][0]=A[2];
  u[1][1]=A[3];
  double w[2];
  svdcmp(u, 2, 2, w, v);

  // A_pinv = V * w_pinv * U_transpose
  // w_pinv is just w with the entries inverted (it's a diagonal square matrix)
  float temp[4];
  temp[0] = v[0][0] * (1.f/w[0]); //temp = V*w_pinv
  temp[2] = v[1][0] * (1.f/w[0]);
  temp[1] = v[0][1] * (1.f/w[1]);
  temp[3] = v[1][1] * (1.f/w[1]);

  A[0] = temp[0]*u[0][0] + temp[1]*u[0][1];
  A[2] = temp[1]*u[0][0] + temp[2]*u[0][1];
  A[1] = temp[0]*u[1][0] + temp[1]*u[1][1];
  A[3] = temp[1]*u[1][0] + temp[2]*u[1][1];
}

// see here for ref: http://www.cs.ucf.edu/~mikel/Research/Optical_Flow.htm
void lucaskanade(
  int width, 
  int height, 
  int windowRadius, 
  unsigned char* frame1, 
  unsigned char* frame2, 
  unsigned char* out){

  float *Ix = new float[width*height];
  float *Iy = new float[width*height];
  float *It = new float[width*height];

  float A[4]; // row major 2x2
  float b[2]; 

  // calculate derivatives

  int border = windowRadius;

  for(int x = border; x < width - border; x++){
    printf("\b\b\b\b\b\b\b\b\b\b%03d / %03d\n",x,width-border);

    for(int y = border; y < height - border; y++){

      // loop over search window
      for( int wx = -windowRadius; wx <= windowRadius; wx++){
	for( int wy = -windowRadius; wy <= windowRadius; wy++){
	  int windex = wy*width+wx;
	  A[0] = A[0] + Ix[windex]*Ix[windex];
	  A[1] = A[1] + Ix[windex]*Iy[windex];
	  A[2] = A[2] + Ix[windex]*Iy[windex];
	  A[3] = A[3] + Iy[windex]*Iy[windex];
	  
	  b[0] = b[0] + Ix[windex]*It[windex];
	  b[1] = b[1] + Iy[windex]*It[windex];
	}
      }

      pinv2by2(A);
      float outX = A[0]*(-b[0])+A[1]*(-b[1]); // result = Ainv * (-b)
      float outY = A[2]*(-b[0])+A[3]*(-b[1]);
      out[3*(y*width+x)]=128+outX*10;
      out[3*(y*width+x)+1]=128+outY*10;

    }
  }

}

int main(int argc, char **argv){

  if(argc!=4){
    printf("Usage: lucaskanade frame1.type frame2.type searchWindowRadius\n");
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
  assert(channels==3);

  unsigned char *frame1 = new unsigned char[width*height];
  unsigned char *frame2 = new unsigned char[width*height];
  unsigned char *out = new unsigned char[width*height*channels];

  toGrayscale(width,height,data,frame1);
  toGrayscale(width,height,data2,frame2);

  saveImage("frame1gray.bmp", width, height, 1, frame1);
  saveImage("frame2gray.bmp", width, height, 1, frame2);

  // zero out vectors array
  for(int i=0; i<width*height*channels; i++){out[i]=0;}

  lucaskanade(width,height,atoi(argv[3]),frame1,frame2,out);

  saveImage("vectors.bmp", width, height, channels, out);

  delete[] frame1;
  delete[] frame2;
  delete[] out;

  return 0;
}
