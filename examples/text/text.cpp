#include "stdio.h"
#include "../util.h"
#include <algorithm>
#include "ispc.h"

void SAT(unsigned char *in, unsigned int *out, int width, int height){
  out[0] = in[0];

  for(int x=1; x<width; x++){
    out[x] = out[x-1]+in[x];
  }

  for(int y=1; y<height; y++){
    out[y*width]=in[y*width];

    for(int x=1; x<width; x++){
      out[x+y*width] = in[x+y*width]+out[(x-1)+(y)*width]+out[(x)+(y-1)*width]-out[(x-1)+(y-1)*width];
    }
  }

}

void SATSquared(unsigned char *in, float *out, int width, int height){
  out[0] = pow(in[0],2);

  for(int x=1; x<width; x++){
    out[x] = out[x-1]+pow(in[x],2);
  }

  for(int y=1; y<height; y++){
    out[y*width]=pow(in[y*width],2);

    for(int x=1; x<width; x++){
      out[x+y*width] = pow(in[x+y*width],2)+out[(x-1)+(y)*width]+out[(x)+(y-1)*width]-out[(x-1)+(y-1)*width];
    }
  }

}

void thresh(unsigned char *in, unsigned int *sat, float *satSq, int width, int height, float k, int w){ // w=window

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){

      int lX = (x-w < 0)?0:x-w;
      int lY = (y-w < 0)?0:y-w;
      int hX = (x+w > width-1)?width-1:x+w;
      int hY = (y+w > height-1)?height-1:y+w;

      float sum = sat[hX+width*hY]-sat[hX+width*lY]-sat[lX+width*hY]+sat[lX+width*lY];
      float sumSq = satSq[hX+width*hY]-satSq[hX+width*lY]-satSq[lX+width*hY]+satSq[lX+width*lY];

      int windSize = (hX-lX)*(hY-lY);

      float mean = sum/windSize;
      float var = sumSq/windSize - pow(mean,2);
      float std=sqrt(var);

      float thresh = mean*(1.f+k*float((std/float(128))-1.f));
      //printf("%f %f %f\n",mean,std,thresh);

      in[x+y*width] = (in[x+y*width]<=thresh)?0:255;
      //in[x+y*width] = thresh;
    }
  }


}

void label(unsigned char *in, unsigned char *out, int width, int height, unsigned int minArea){
  const int blockSize = 8;

  unsigned char equivClass[256];
  bool live[256];
  unsigned int area[256];
  unsigned char labelStack[256];
  int labelStackPosition = -1;
  
  for(int i=0; i<256; i++){
    equivClass[i]=0;
    live[i]=false;
    area[i]=0;
    labelStack[i]=i+1;
  }
  labelStackPosition = 254;

  //  for(int block = 0; block < height/blockSize; block++){
  for(int block = 0; block < 1; block++){

    for(int y = block*blockSize; y<height && y <(block+1)*blockSize; y++){

      for(int x=0; x<width; x++){

	if(in[x+y*width]){
	  if(x==0 && y==0){
	    out[x+y*width] = labelStack[labelStackPosition];
	    //live[out[x+y*width]]=true; // dont care about liveness on first row
	    area[out[x+y*width]]++;
	    labelStackPosition--;
	    assert(labelStackPosition>=0);
	  }else if(x==0){
	    if(in[x+(y-1)*width]){
	      out[x+y*width] = out[x+(y-1)*width];
	      live[out[x+y*width]]=true;
	      area[out[x+y*width]]++;
	    }else{
	      out[x+y*width] = labelStack[labelStackPosition];
	      live[out[x+y*width]]=true;
	      area[out[x+y*width]]++;
	      labelStackPosition--;
	      assert(labelStackPosition>=0);
	    }
	  }else if(y==0){
	    if(in[x-1]){
	      out[x]=out[x-1];
	      area[out[x]]++;
	    }else{
	      out[x+y*width] = labelStack[labelStackPosition];
	      //live[out[x+y*width]]=0; // don't care about liveness on first row
	      area[out[x+y*width]]++;
	      labelStackPosition--;
	      assert(labelStackPosition>=0);
	    }
	  }else{
	    if(in[x+(y-1)*width] && in[(x-1)+y*width]){
	      if(out[x+(y-1)*width]!=out[(x-1)+y*width]){
		out[x+y*width] = std::min(out[(x-1)+y*width],out[x+(y-1)*width]);
		live[out[x+y*width]]=true;
		area[out[x+y*width]]++;
		int mx = std::max(out[(x-1)+y*width],out[x+(y-1)*width]);

		if(equivClass[mx]!=0 && equivClass[mx]!=out[x+y*width]){
		  printf("%d %d %d %d\n",equivClass[mx],out[(x-1)+y*width],out[x+(y-1)*width],mx);
		}

		assert(equivClass[mx]==0 || equivClass[mx]==out[x+y*width]);

		equivClass[mx] = out[x+y*width];
	      }else{
		out[x+y*width] = out[x+(y-1)*width];
		live[out[x+y*width]]=true;
		area[out[x+y*width]]++;
	      }
	    }else if(in[x+(y-1)*width] || in[(x-1)+y*width]){
	      out[x+y*width] = out[x+(y-1)*width] | out[(x-1)+y*width];
	      live[out[x+y*width]]=true;
	      area[out[x+y*width]]++;
	    }else{
	      out[x+y*width] = labelStack[labelStackPosition];
	      live[out[x+y*width]]=true;
	      area[out[x+y*width]]++;
	      labelStackPosition--;
	      assert(labelStackPosition>=0);
	    }
	  }

	}else{
	  out[x+y*width]=0;
	}
      }
      
      // clear out dead stuff (stuff that's too small) from last row
      // note: we are now aliasing the live variable w/ a variable that stores stuff to delete
      if(y>0){
	for(int i=0; i<256; i++){
	  //printf("%d %d %d %d\n",i,live[i],live[equivClass[i]],area[i]);
	  if(!live[i] && (!live[equivClass[i]] || equivClass[i]==0) && area[i]<minArea){
	    //assert(equivClass[i]==0);
	    // I think this works, because if either (a) i was larger than the equiv classes on join, or (b) i was smaller
	    // if (a) then equivClass should point to the right liveness
	    // if (b) then live[i] was set true b/c it's value was picked up

	    live[i]=true;
	    //printf("delete %d\n",i);
	  }else{
	    live[i]=false;
	  }
	}
	
	for(int x=width-1; x>=0; x--){
	  if(live[out[x+(y-1)*width]]){
	    out[x+(y-1)*width]=0;
	    in[x+(y-1)*width]=0;
	  }

	  // can't do this here
	  /*
	  int eq = equivClass[out[x+(y-1)*width]];
	  if(eq!=0){
	    out[x+(y-1)*width]=eq;
	  }
	  */
	}
	
	for(int i=0; i<256; i++){
	  if(live[i]){
	    live[i]=false;
	    area[i]=0;
	    labelStackPosition++;
	    labelStack[labelStackPosition]=i;
	    equivClass[i]=0;
	  }
	}
      }
    }
    
  }
}

void invert(unsigned char *in, int width, int height){
  for(int x=1; x<width; x++){
    for(int y=1; y<height; y++){
      in[x+y*width] = (in[x+y*width]>0)?0:255;
    }
  }
}

int main(int argc, char **argv){

  if(argc != 4){
    printf("wrong # args\n");
  }

  int width, height, channels;
  unsigned char *data;
  float k = float(atoi(argv[2]))/1000.f;
  int window = atoi(argv[3]);

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  if(!res){
    printf("error reading image\n");
    return 1;
  }

  // to grayscale
  unsigned char *dataGray = new unsigned char[width*height];
  toGrayscale(width,height,data,dataGray);

  // build SAT
  unsigned int *sat = new unsigned int[width*height];
  float * satSquared = new float[width*height];
  SAT(dataGray,sat,width,height);
  SATSquared(dataGray,satSquared,width,height);

  // do thresh
  thresh(dataGray, sat, satSquared, width, height, k, window);
  invert(dataGray,width,height);

  saveImage("./resRaw.bmp", width, height, 1, dataGray);

  // dist trans
  unsigned char *upVec = new unsigned char[width*height];
  unsigned char *downVec = new unsigned char[width*height];
  unsigned char *leftVec = new unsigned char[width*height];
  unsigned char *rightVec = new unsigned char[width*height];
  ispc::dist(dataGray, upVec, downVec, leftVec, rightVec, width, height);
  saveImage("./upVec.bmp", width, height, 1, upVec);
  saveImage("./downVec.bmp", width, height, 1, downVec);
  saveImage("./leftVec.bmp", width, height, 1, leftVec);
  saveImage("./rightVec.bmp", width, height, 1, rightVec);

  // label
  unsigned char *L = new unsigned char[width*height];
  unsigned char *temp = new unsigned char[width*height];
  //label(dataGray,L,width,height,30);

  ispc::simple(dataGray,upVec,downVec,leftVec,rightVec,temp,L,width,height);

  // write out
  saveImage("./res.bmp", width, height, 1, dataGray);
  saveImage("./L.bmp", width, height, 1, L);

  return 0;
}
