#include "stdio.h"
#include "../util.h"
#include <algorithm>
#include "ispc.h"
#include "../distancetransform/distancetransform.h"
#include "../label/label.h"

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
  int startY, int endY){

  unsigned char *in = pyramid[L];

  unsigned short area[maxLabels];
  computeArea(labels,area,width,height,startX,endX,startY,endY);

  unsigned short l[maxLabels];
  unsigned short r[maxLabels];
  unsigned short t[maxLabels];
  unsigned short b[maxLabels];
  computeBB(labels,l,r,t,b,width,height,startX,endX,startY,endY);

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
      
      printf("%d %f %d %f\n",i,ratio,area[i],solidity);
      
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
	(*out)[L][x+y*lw]=(cnt>=2)?255:0; // used to be >, but this makes larger regions
      }
    }
  }
}

int main(int argc, char **argv){

  if(argc != 5){
    printf("wrong # args\n");
  }

  int width, height, channels;
  unsigned char *data;
  int inv = atoi(argv[2]);
  float k = float(atoi(argv[3]))/1000.f;
  int window = atoi(argv[4]);

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
  if(inv){invert(dataGray,width,height);}

  saveImage("./resRaw.bmp", width, height, 1, dataGray);

  unsigned char ** pyramid=NULL;
  makePyramid(dataGray, &pyramid, 4, width,height);

  for(int L=0; L<4; L++){
    char filename[20];
    sprintf(filename,"res_raw_%d.bmp",L);
    saveImage(filename,width/pow(2,L),height/pow(2,L),1,pyramid[L]);
  }


  // label
  int vecSizeY=32;
  int vecSizeX=128;

  unsigned char *dist=new unsigned char[width*height];
  unsigned short *labels = new unsigned short[width*height];
  unsigned short area[maxLabels];
  unsigned char *reason = new unsigned char[width*height];
  unsigned char *tile = new unsigned char[width*height];
  
  for(int l = 3; l>=0; l--){
    int lw = width/pow(2,l);
    int lh = height/pow(2,l);
    
    // write out debug data
    char filename[20];
    sprintf(filename,"res_%dp2.bmp",l);
    saveImage(filename, lw,lh, 1, pyramid[l]);
  }

  for(int L = 3; L>=0; L--){
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

	label(pyramid[L],labels,reason,area,lw,lh,minArea,vecSizeX*bx,endX,vecSizeY*by,endY);
	unsigned short lastId=0;
	if(!condenseLabels(labels,area,firstId,&lastId,lw,lh,vecSizeX*bx,endX,vecSizeY*by,endY)){
	  printf("Error: ran out of labels!\n");
	  goto writeout;  // an exception occured
	}
	firstId=lastId+1;
	printf("ML %d\n",(int)firstId);
	
	// for debugging
	for(int x=vecSizeX*bx; x<endX; x++){
	  for(int y=vecSizeY*by; y<endY; y++){
	    tile[x+y*lw]=tileId;
	  }
	}

	tileId++;

	if(firstId > (maxLabels*4)/5 && !fixupRanThisRow){
	  labelProp(labels,area,lw,lh,0,lw,0,endY);
	  filter(pyramid,L,reason,labels,minArea,lw,lh,width,0,lw,lastFixup,endY);
	  computeArea(labels,area,lw,lh,0,lw,0,endY);
	  condenseLabels(labels,area,1,&firstId,lw,lh,0,lw,0,endY);
	  firstId++;
	  printf("fixup TL %d\n",firstId);
	  
	  lastFixup = (by+1)*vecSizeY;
	  fixupRanThisRow=true;
	}
	
      }

    }
    
    // fix labels across region boundries
    labelProp(labels,area,lw,lh,0,lw,0,lh);
    computeArea(labels,area,lw,lh,0,lw,0,lh);
    condenseLabels(labels,area,1,&firstId,lw,lh,0,lw,0,lh);
    printf("TL %d\n",firstId);
    
    distanceTransform(pyramid[L],dist,0,lw,lh);
    
    filter(pyramid,L,reason,labels,minArea,lw,lh,width,0,lw,0,lh);

    // now what's left "must" be text, so delete it from other pyrmid levels and save it
    
    writeout:
    // write out debug data
    char filename[20];
    sprintf(filename,"L_%d.bmp",L);
    saveImage(filename, lw,lh, labels);
    sprintf(filename,"D_%d.bmp",L);
    saveImage(filename, lw,lh, 1, dist);
    sprintf(filename,"res_%d.bmp",L);
    saveImage(filename, lw,lh, 1, pyramid[L]);
    sprintf(filename,"R_%d.bmp",L);
    saveImage(filename, lw,lh, 1, reason);
    sprintf(filename,"T_%d.bmp",L);
    saveImage(filename, lw,lh, 1, tile);

    if(L==3){
      for(int l = 2; l>=0; l--){
	int lw = width/pow(2,l);
	int lh = height/pow(2,l);
	
	// write out debug data
	char filename[20];
	sprintf(filename,"res_%dp.bmp",l);
	saveImage(filename, lw,lh, 1, pyramid[l]);
      }
    }
    
  }
  
  for(int y=0; y<height; y++){
    for(int x=0; x<width; x++){
      int count=0;
      for(int l=0; l<4; l++){
	int lx = x/pow(2,l);
	int ly = y/pow(2,l);

	int lw = width/pow(2,l);

	if(pyramid[l][lx+ly*lw]){count++;}
      }
      if(count<2){
	dataGray[x+y*width]=0;
      }
    }
  }

  saveImage("./res.bmp", width, height, 1, dataGray);

  return 0;
}
