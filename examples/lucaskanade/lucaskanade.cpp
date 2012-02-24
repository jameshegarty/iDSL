#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "svdcmp.h"

// calculate the pseudo inverse in place
void pinv2by2(float *A){
  double **u = dmatrix(1,2,1,2);
  double **v = dmatrix(1,2,1,2);
  u[1][1]=A[0];
  u[1][2]=A[1];
  u[2][1]=A[2];
  u[2][2]=A[3];
  double w[3];
  svdcmp(u, 2, 2, w, v);

  // A_pinv = V * w_pinv * U_transpose
  // w_pinv is just w with the entries inverted (it's a diagonal square matrix)
  float temp[4];
  temp[0] = v[1][1] * (1.f/w[1]); //temp = V*w_pinv
  temp[2] = v[2][1] * (1.f/w[1]);
  temp[1] = v[1][2] * (1.f/w[2]);
  temp[3] = v[2][2] * (1.f/w[2]);

  A[0] = temp[0]*u[1][1] + temp[1]*u[1][2];
  A[2] = temp[2]*u[1][1] + temp[3]*u[1][2];
  A[1] = temp[0]*u[2][1] + temp[1]*u[2][2];
  A[3] = temp[2]*u[2][1] + temp[3]*u[2][2];

  free_dmatrix(u,1,2,1,2);
  free_dmatrix(v,1,2,1,2);
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
  for(int x = 1; x < width - 1; x++){
    for(int y = 1; y < height - 1; y++){
      Ix[y*width+x] = (float(frame1[(y-1)*width+x+1])-float(frame1[(y-1)*width+x-1]));
      Ix[y*width+x] += (float(frame1[(y+1)*width+x+1])-float(frame1[(y+1)*width+x-1]));
      Ix[y*width+x] /= 8.f;

      Ix[y*width+x] = (float(frame1[y*width+x+1])-float(frame1[y*width+x-1]))/2.f;

      Iy[y*width+x] = (float(frame1[(y+1)*width+x-1])-float(frame1[(y-1)*width+x-1]));
      Iy[y*width+x] += (float(frame1[(y+1)*width+x+1])-float(frame1[(y-1)*width+x+1]));
      Iy[y*width+x] /= 8.f;

      Iy[y*width+x] = (float(frame1[(y+1)*width+x])-float(frame1[(y-1)*width+x]))/2.f;

      It[y*width+x] = (float(frame2[y*width+x])-float(frame1[y*width+x]));
    }
  }

  // do LK calculation

  int border = windowRadius;

  for(int x = border; x < width - border; x++){
    printf("\b\b\b\b\b\b\b\b\b\b%03d / %03d\n",x,width-border);

    for(int y = border; y < height - border; y++){
      
      A[0] = 0.f;
      A[1] = 0.f;
      A[2] = 0.f;
      A[3] = 0.f;
      b[0] = 0.f;
      b[1] = 0.f;

      // loop over search window
      for( int wx = -windowRadius; wx <= windowRadius; wx++){
	for( int wy = -windowRadius; wy <= windowRadius; wy++){
	  int windex = (y+wy)*width+x+wx;
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

  ////
  float A[4];
  A[0] = -7;
  A[1] = 26;
  A[2] = 14;
  A[3] = 0;
  pinv2by2(A);
  printf("%f %f %f %f                            \n\n\n",A[0], A[1],A[2],A[3]);
  ///

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
