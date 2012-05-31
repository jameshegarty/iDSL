#include "stdio.h"
#include "../util.h"
#include <algorithm>
#include "ispc.h"
#include "../distancetransform/distancetransform.h"
#include "../label/label.h"

const int pyramidLevels=4;
const int maxLabels = 1024;

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

void invert(unsigned char *in, int width, int height){
  for(int x=1; x<width; x++){
    for(int y=1; y<height; y++){
      in[x+y*width] = (in[x+y*width]>0)?0:255;
    }
  }
}

void filter(
  unsigned char **pyramid, 
  int L, // pyramid level
  unsigned char *reason,
  unsigned short *labels, 
  int minArea,
  int width, int height,
  int owidth,
  int startX, int endX,
  int startY, int endY,
  int maxLabels){

  unsigned char *in = pyramid[L];

  unsigned short area[maxLabels];
  computeArea(labels,area,width,height,startX,endX,startY,endY,maxLabels);

  unsigned short l[maxLabels];
  unsigned short r[maxLabels];
  unsigned short t[maxLabels];
  unsigned short b[maxLabels];
  computeBB(labels,l,r,t,b,width,height,startX,endX,startY,endY,maxLabels);

  bool kill[maxLabels];
  unsigned char kreason[maxLabels];

  for(int i=0; i<maxLabels; i++){
    if(area[i]>0 && i>0){ // is this region still there?

      // ratio
      int w = (r[i]-l[i])+1;
      int h = (t[i]-b[i])+1;
      float ratio = float(w)/float(h);
      
      //solidity
      float solidity = float(area[i])/float(w*h);
      
      //printf("%d %f %d %f\n",i,ratio,area[i],solidity);
      
      float areaPercent = float(area[i])/float(width*height);
      
      if(ratio < 0.1f || ratio > 2.f){
	kill[i] = true;
	kreason[i]=50;
      }else if(area[i]<minArea){
	kill[i] = true;
	kreason[i]=100;
      }else if(areaPercent>0.2f){
	kill[i] = true;
	kreason[i]=150;
      }else if(solidity < 0.20f){
	kill[i] = true;
	kreason[i]=200;
      }else{
	kill[i]=false;
	kreason[i]=0;
      }
      
    }else{
      kill[i] = false;
      kreason[i]=0;
    }
  }


  for(int x=startX; x<endX; x++){
    for(int y=startY; y<endY; y++){
      if(kill[labels[x+y*width]]){
	in[x+y*width] = 0;
	unsigned char r = kreason[labels[x+y*width]];
	reason[x+y*width]=r;
	labels[x+y*width]=0;

	if(r==150){
	  int sX = x*2;
	  int eX = x*2+2;
	  int sY = y*2;
	  int eY = y*2+2;

	  for(int l=L-1; l>=0; l--){
	    int lw=owidth/pow(2,l);

	    for(int i=sX; i<eX; i++){
	      for(int j=sY; j<eY; j++){
		pyramid[l][i+j*lw]=0;
	      }
	    }
	    sX*=2;
	    sY*=2;
	    eX*=2;
	    eY*=2;
	  }
	}

      }
    }
  }

  // keep area accurate
  for(int i=0; i<maxLabels;i++){
    if(kill[i]){area[i]=0;}
  }
}

void makePyramid(
  unsigned char *in, unsigned char ***out, int desiredLevels,
  int thresh,
  int width, int height){

  // build space
  *out = new unsigned char*[desiredLevels];
  for(int i=0; i<desiredLevels; i++){ (*out)[i]=new unsigned char[width*height/(int)pow(2,2*i)];}

  memcpy((*out)[0],in,width*height);

  // build levels
  for(int L=1; L<desiredLevels; L++){
    int lw = width/pow(2,L);
    int lh = height/pow(2,L);

    int lwm1 = width/pow(2,L-1);
    
    for(int x=0; x<lw; x++){
      for(int y=0; y<lh; y++){
	int cnt=0;
	cnt = ((*out)[L-1][(x*2)+(y*2)*lwm1])?cnt+1:cnt;
	cnt = ((*out)[L-1][(x*2+1)+(y*2)*lwm1])?cnt+1:cnt;
	cnt = ((*out)[L-1][(x*2)+(y*2+1)*lwm1])?cnt+1:cnt;
	cnt = ((*out)[L-1][(x*2+1)+(y*2+1)*lwm1])?cnt+1:cnt;
	(*out)[L][x+y*lw]=(cnt>=thresh)?255:0; // used to be >, but this makes larger regions
      }
    }
  }
}

int main(int argc, char **argv){

  if(argc != 9){
    printf("wrong # args\n");
    printf("inputfile outDir(./lol/) invert (0 or 1) k*1000 windowRadius gaussPyramidThresh(>= survive) finalMixThresh(>= survive) writeDebugImages(0 or 1)\n");
  }

  int width, height, channels;
  unsigned char *data;
  char *dir = argv[2];
  int inv = atoi(argv[3]);
  float k = float(atoi(argv[4]))/1000.f;
  int window = atoi(argv[5]);
  int gaussPyramidThresh = atoi(argv[6]);
  int finalMixThresh = atoi(argv[7]);
  bool debugImages = atoi(argv[8]);

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  if(!res){
    printf("error reading image\n");
    return 1;
  }

  printf("start\n");

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
  if(inv){invert(dataGray,width,height);}

  char filename[200];
  sprintf(filename,"%sresRaw.bmp",dir);
  if(debugImages){saveImage(filename, width, height, 1, dataGray);}

  unsigned char ** pyramid=NULL;
  makePyramid(dataGray, &pyramid, pyramidLevels, gaussPyramidThresh, width,height);

  for(int L=0; L<pyramidLevels; L++){
    char filename[200];
    sprintf(filename,"%sres_raw_%d.bmp",dir,L);
    if(debugImages){saveImage(filename,width/pow(2,L),height/pow(2,L),1,pyramid[L]);}
  }


  // label
  int vecSizeY=32;
  int vecSizeX=128;

  unsigned char *dist=new unsigned char[width*height];
  unsigned short *labels = new unsigned short[width*height];
  unsigned short area[maxLabels];
  unsigned char *reason = new unsigned char[width*height];
  unsigned char *tile = new unsigned char[width*height];
  
  for(int l = pyramidLevels-1; l>=0; l--){
    int lw = width/pow(2,l);
    int lh = height/pow(2,l);
    
    // write out debug data
    char filename[200];
    sprintf(filename,"%sres_%dp2.bmp",dir,l);
    if(debugImages){saveImage(filename, lw,lh, 1, pyramid[l]);}
  }

  for(int L = pyramidLevels-1; L>=0; L--){
    int lw = width/pow(2,L);
    int lh = height/pow(2,L);

    // clear out labels so that we can do progressive cleanup passes
    for(int i=0; i<lw*lh; i++){reason[i]=0;labels[i]=0;}

    unsigned short firstId = 1;

    int tileId = 0;

    int minArea = 6;
    if(L<2){minArea = 30;}

    int nTilesX = ceil((float)lw/(float)vecSizeX);
    int nTilesY = ceil((float)lh/(float)vecSizeY);

    int lastFixup = 0;

    for(int by=0; by<nTilesY; by++){
      int endY = (by+1)*vecSizeY;
      if(endY>lh){endY=lh;}
      
      bool fixupRanThisRow = false;

      for(int bx=0; bx<nTilesX; bx++){

	int endX = (bx+1)*vecSizeX;
	if(endX>lw){endX=lw;}

	label(pyramid[L],labels,reason,area,lw,lh,minArea,vecSizeX*bx,endX,vecSizeY*by,endY,maxLabels);
	unsigned short lastId=0;
	if(!condenseLabels(labels,area,firstId,&lastId,lw,lh,vecSizeX*bx,endX,vecSizeY*by,endY,maxLabels)){
	  printf("Error: ran out of labels!\n");
	  goto writeout;  // an exception occured
	}
	firstId=lastId+1;
	//printf("ML %d\n",(int)firstId);
	
	// for debugging
	for(int x=vecSizeX*bx; x<endX; x++){
	  for(int y=vecSizeY*by; y<endY; y++){
	    tile[x+y*lw]=tileId;
	  }
	}

	tileId++;

	if(firstId > (maxLabels*4)/5 && !fixupRanThisRow){
	  labelProp(labels,area,lw,lh,0,lw,0,endY,maxLabels);
	  filter(pyramid,L,reason,labels,minArea,lw,lh,width,0,lw,lastFixup,endY,maxLabels);
	  computeArea(labels,area,lw,lh,0,lw,0,endY,maxLabels);
	  condenseLabels(labels,area,1,&firstId,lw,lh,0,lw,0,endY,maxLabels);
	  firstId++;
	  printf("fixup TL %d\n",firstId);
	  
	  lastFixup = (by+1)*vecSizeY;
	  fixupRanThisRow=true;
	}
	
      }

    }
    
    // fix labels across region boundries
    labelProp(labels,area,lw,lh,0,lw,0,lh,maxLabels);
    computeArea(labels,area,lw,lh,0,lw,0,lh,maxLabels);
    condenseLabels(labels,area,1,&firstId,lw,lh,0,lw,0,lh,maxLabels);
    //printf("TL %d\n",firstId);
    
    distanceTransform(pyramid[L],dist,0,lw,lh);
    
    filter(pyramid,L,reason,labels,minArea,lw,lh,width,0,lw,0,lh,maxLabels);

    // now what's left "must" be text, so delete it from other pyrmid levels and save it
    
    writeout:
    // write out debug data
    char filename[200];
    if(debugImages){
      sprintf(filename,"%sL_%d.bmp",dir,L);
      saveImage(filename, lw,lh, labels);
      sprintf(filename,"%sD_%d.bmp",dir,L);
      saveImage(filename, lw,lh, 1, dist);
      sprintf(filename,"%sres_%d.bmp",dir,L);
      saveImage(filename, lw,lh, 1, pyramid[L]);
      sprintf(filename,"%sR_%d.bmp",dir,L);
      saveImage(filename, lw,lh, 1, reason);
      sprintf(filename,"%sT_%d.bmp",dir,L);
      saveImage(filename, lw,lh, 1, tile);
    }

    if(L==pyramidLevels-1){
      for(int l = 2; l>=0; l--){
	int lw = width/pow(2,l);
	int lh = height/pow(2,l);
	
	// write out debug data
	char filename[200];
	sprintf(filename,"%sres_%dp.bmp",dir,l);
	if(debugImages){saveImage(filename, lw,lh, 1, pyramid[l]);}
      }
    }
    
  }
  
  for(int y=0; y<height; y++){
    for(int x=0; x<width; x++){
      int count=0;
      for(int l=0; l<pyramidLevels; l++){
	int lx = x/pow(2,l);
	int ly = y/pow(2,l);

	int lw = width/pow(2,l);

	if(pyramid[l][lx+ly*lw]){count++;}
      }
      if(count<finalMixThresh){
	dataGray[x+y*width]=0;
      }
    }
  }

  sprintf(filename,"%sres.bmp",dir);
  saveImage(filename, width, height, 1, dataGray);

  return 0;
}
