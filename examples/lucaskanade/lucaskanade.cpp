#include "../util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <algorithm>

#include "svdcmp.h"

bool near(double a, double b){
  return abs(1.f-(a/b)) < 0.001f;
}

// calculate the pseudo inverse in place
// if singular return a matrix of zeros
void pinv2by2(float *A){

  if(A[0]*A[3]-A[1]*A[2] == 0){

    printf("Singular\n");
    A[0]=0;
    A[1]=0;
    A[2]=0;
    A[3]=0;

    return;
  }

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
  double temp[4];
  temp[0] = v[1][1] * (1.f/w[1]); //temp = V*w_pinv
  temp[2] = v[2][1] * (1.f/w[1]);
  temp[1] = v[1][2] * (1.f/w[2]);
  temp[3] = v[2][2] * (1.f/w[2]);

  double R[4];

  R[0] = temp[0]*u[1][1] + temp[1]*u[1][2];
  R[2] = temp[2]*u[1][1] + temp[3]*u[1][2];
  R[1] = temp[0]*u[2][1] + temp[1]*u[2][2];
  R[3] = temp[2]*u[2][1] + temp[3]*u[2][2];

  {  // varify correctness
    double S[4];
    S[0] = R[0]*A[0]+R[1]*A[2]; // = A_pseudo * A
    S[1] = R[0]*A[1]+R[1]*A[3];
    S[2] = R[2]*A[0]+R[3]*A[2];
    S[3] = R[2]*A[1]+R[3]*A[3];

    double AApA[4];
    AApA[0] = A[0]*S[0]+A[1]*S[2];
    AApA[1] = A[0]*S[1]+A[1]*S[3];
    AApA[2] = A[2]*S[0]+A[3]*S[2];
    AApA[3] = A[2]*S[1]+A[3]*S[3];

    // A * A_pseudo * A == A
    if(!(near(AApA[0],A[0]) && 
	 near(AApA[1],A[1]) && 
	 near(AApA[2],A[2]) && 
	 near(AApA[3],A[3]))){
      
      printf("%f %f %f %f\n",A[0],A[1],A[2],A[3]);
      printf("pinv %f %f %f %f\n",R[0],R[1],R[2],R[3]);
      printf("%f %f %f %f\n",S[0],S[1],S[2],S[3]);
      printf("AApA %f %f %f %f\n",AApA[0],AApA[1],AApA[2],AApA[3]);
      printf("w %f %f\n",w[1],w[2]);
      assert(false);
    }
  }

  A[0] = R[0];
  A[1] = R[1];
  A[2] = R[2];
  A[3] = R[3];

  free_dmatrix(u,1,2,1,2);
  free_dmatrix(v,1,2,1,2);
}

// see here for ref: http://www.cs.ucf.edu/~mikel/Research/Optical_Flow.htm
// output should have 3 channels (size width*height*3)
void lucaskanade(
  int inWidth, 
  int inHeight, 
  int windowRadius, 
  int iterations,
  int pyramidLevels,
  bool weighted,
  unsigned char* frame1in, 
  unsigned char* frame2in, 
  float* out){
  
  assert(pyramidLevels > 0);
  int border = windowRadius;

  unsigned char **frame1Pyramid = NULL;
  unsigned char **frame2Pyramid = NULL;

  buildPyramid(inWidth,inHeight,pyramidLevels,frame1in,&frame1Pyramid);
  buildPyramid(inWidth,inHeight,pyramidLevels,frame2in,&frame2Pyramid);

  // zero out vectors array. 127 = 0 in our stupid fixed precision format
  for(int x = 0; x<inWidth; x++){
    for(int y = 0; y<inHeight; y++){
      out[(y*inWidth+x)*2+0]=0.f;
      out[(y*inWidth+x)*2+1]=0.f;
    }
  }

  float *Fx = new float[inWidth*inHeight];
  float *Fy = new float[inWidth*inHeight];
  //float *A = new float[inWidth*inHeight*4];
  float *A[4];
  for(int i=0; i<4; i++){A[i]=new float[inWidth*inHeight];}
  float *W = new float[inWidth*inHeight];
  float *outTemp = new float[inWidth*inHeight*2];

  for(int l=pyramidLevels-1; l>=0; l--){
    // calculate stuff that we will use every iteration
    // such as derivatives, matrix A^-1 of image gradients, weights.
    // we're calling frame1 F and frame2 G, as in the original LK paper
    
    unsigned char *frame1 = frame1Pyramid[l];
    unsigned char *frame2 = frame2Pyramid[l];

    int width = inWidth / pow(2,l);
    int height = inHeight / pow(2,l);

    for(int x = 0; x < width; x++){
      for(int y = 0; y < height; y++){
	// calculate derivatives
	float f1xp1 = sampleNearestClamped(width, height, x+1, y, frame1);
	float f1xm1 = sampleNearestClamped(width, height, x-1, y, frame1);
	float f1yp1 = sampleNearestClamped(width, height, x, y+1, frame1);
	float f1ym1 = sampleNearestClamped(width, height, x, y-1, frame1);
	Fx[y*width+x] = (f1xp1-f1xm1)/2.f;
	Fy[y*width+x] = (f1yp1-f1ym1)/2.f;

	float f2xp1 = sampleNearestClamped(width, height, x+1, y, frame2);
	float f2xm1 = sampleNearestClamped(width, height, x-1, y, frame2);
	float f2yp1 = sampleNearestClamped(width, height, x, y+1, frame2);
	float f2ym1 = sampleNearestClamped(width, height, x, y-1, frame2);	
	float Gx = (f2xp1-f2xm1)/2.f;
	float Gy = (f2yp1-f2ym1)/2.f;
	
	// calculate weight W
	float w = sqrt(pow(Gx-Fx[y*width+x],2)+pow(Gy-Fy[y*width+x],2));
	if(w!=0.f){w = 1.f/w;}else{w=100.f;}
	if(!weighted){w = 1.f;}
	W[y*width+x] = w;
      }
    }
    
    float Atemp[4]; // row major 2x2
    // calculate A^-1. notice, can't combine this with loop above
    // b/c it requires all values to already be calculated in diff window
    for(int x = 0; x < width; x++){
      for(int y = 0; y < height; y++){
	Atemp[0] = 0.f;
	Atemp[1] = 0.f;
	Atemp[2] = 0.f;
	Atemp[3] = 0.f;

	for( int wx = -windowRadius; wx <= windowRadius; wx++){
	  for( int wy = -windowRadius; wy <= windowRadius; wy++){

	    float dx = sampleNearestClamped(width, height, x+wx, y+wy, Fx);
	    float dy = sampleNearestClamped(width, height, x+wx, y+wy, Fy);
	    float w = sampleNearestClamped(width, height, x+wx, y+wy, W);

	    Atemp[0] = Atemp[0] + dx*dx*w;
	    Atemp[1] = Atemp[1] + dx*dy*w;
	    Atemp[2] = Atemp[2] + dx*dy*w;
	    Atemp[3] = Atemp[3] + dy*dy*w;	  
	  }
	}

	pinv2by2(Atemp);

	A[0][y*width+x]=Atemp[0];
	A[1][y*width+x]=Atemp[1];
	A[2][y*width+x]=Atemp[2];
	A[3][y*width+x]=Atemp[3];
      }
    }

    /*char tstr[100];
    sprintf(tstr,"dx_%d.bmp",l);
    saveImageAutoLevels(tstr, width, height, 1, Fx);
    sprintf(tstr,"dy_%d.bmp",l);
    saveImageAutoLevels(tstr, width, height, 1, Fy);
    sprintf(tstr,"w_%d.bmp",l);
    saveImage(tstr, width, height, 1, W);
    sprintf(tstr,"a0_%d.bmp",l);
    saveImage(tstr, width, height, 1, A[0]);
    sprintf(tstr,"a1_%d.bmp",l);
    saveImage(tstr, width, height, 1, A[0]);
    sprintf(tstr,"a2_%d.bmp",l);
    saveImage(tstr, width, height, 1, A[0]);
    sprintf(tstr,"a3_%d.bmp",l);
    saveImage(tstr, width, height, 1, A[0]);
    */

    // do LK calculation
    /* Notice: instead of iterating the same # of times for each pixel,
       we could instead iterate a different # of times for each pixel 
       (until the error < epsilon for ex). This would prob make for better
       results, but wouldn't be parallelizable
    */
    for(int i = 0; i<iterations; i++){
      for(int x = 0; x < width; x++){
	printf("\b\b\b\b\b\b\b\b\b\b%03d / %03d\n",x,width-border);
      
	for(int y = 0; y < height; y++){

	  //	  if( y < 100){continue;}

	  float b[2]; 
	  b[0] = 0.f;
	  b[1] = 0.f;

	  float hx = out[2*(y*width+x)+0];
	  float hy = out[2*(y*width+x)+1];

	  //	  float wsum = 0.f;

	  // loop over search window
	  for( int wx = -windowRadius; wx <= windowRadius; wx++){
	    for( int wy = -windowRadius; wy <= windowRadius; wy++){

	      float dx = sampleNearestClamped( width, height, x+wx, y+wy, Fx );
	      float dy = sampleNearestClamped( width, height, x+wx, y+wy, Fy );
	      float w = sampleNearestClamped( width, height, x+wx, y+wy, W );

	      /*
	      if(x+wx < 0){assert(w==0.f);}
	      if(x+wx >width-1){assert(w==0.f);}
	      if(y+wy < 0){assert(w==0.f);}
	      if(y+wy >height-1){assert(w==0.f);}
	      */

	      //	      wsum += w;

	      float F = sampleNearestClamped( width, height, x+wx, y+wy, frame1 );
	      float G = sampleBilinearClamped( width, height, x+wx+hx, y+wy+hy, frame2 );
	    
	      b[0] = b[0] + dx*(G-F)*w;
	      b[1] = b[1] + dy*(G-F)*w;
	    }
	  }
	  //	  b[0]*=pow(2*windowRadius+1,2)/wsum;
	  //	  b[1]*=pow(2*windowRadius+1,2)/wsum;

	  float outX = A[0][y*width+x]*(-b[0])+A[1][y*width+x]*(-b[1]); // result = Ainv * (-b)
	  float outY = A[2][y*width+x]*(-b[0])+A[3][y*width+x]*(-b[1]);
	  out[2*(y*width+x)]=outX+hx;
	  out[2*(y*width+x)+1]=outY+hy;
	}
      }
    }

    // upsample vector field for next level
    /*
    sprintf(tstr,"f1_%d.bmp",l);
    saveImage(tstr, width, height, 1, frame1);
    sprintf(tstr,"f2_%d.bmp",l);
    saveImage(tstr, width, height, 1, frame2);
    sprintf(tstr,"pv_%d.bmp",l);
    saveImage(tstr, width, height, 3, out);
    */
    if(l>0){
      for(int x = 0; x < width*2; x++){
	for(int y = 0; y < height*2; y++){
	  sampleBilinear2Channels(width,height, float(x)/2.f, float(y)/2.f, out, &outTemp[(y*width*2+x)*2]);
          outTemp[(y*width*2+x)*2+0] *= 2.f;
          outTemp[(y*width*2+x)*2+1] *= 2.f;
	}
      }

      memcpy(out,outTemp,(2*width)*(2*height)*2*sizeof(float));

      /*
      sprintf(tstr,"pv_up_%d.bmp",l);
      saveImage(tstr, width*2, height*2, 3, out);
      */
    }
  }
}

int main(int argc, char **argv){

  if(argc!=7 && argc!=8){
    printf("Usage: lucaskanade frame1.type frame2.type out.bmp searchWindowRadius iterations pyramidLevels [-w]\n");
    printf("add -w on the end to use the algorithm with weights\n");
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
  float *out = new float[width*height*2];

  toGrayscale(width,height,data,frame1);
  toGrayscale(width,height,data2,frame2);

  //saveImage("frame1gray.bmp", width, height, 1, frame1);
  //saveImage("frame2gray.bmp", width, height, 1, frame2);

  lucaskanade(width,height,atoi(argv[4]),atoi(argv[5]),atoi(argv[6]),argc==8,frame1,frame2,out);

  saveImage(argv[3], width, height, 2, out);

  delete[] frame1;
  delete[] frame2;
  delete[] out;

  return 0;
}
