#include "stdio.h"
#include "../util.h"
#include <algorithm>
#include "ispc.h"
#include "../distancetransform/distancetransform.h"

const int maxLabels = 512;

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

void labelProp(
  unsigned short *out, 
  unsigned short area[maxLabels],
  int width, int height,
  int startX, int endX,
  int startY, int endY){


  // prop y down
  unsigned short rename[maxLabels]; // atomic
  for(int i=0; i<maxLabels; i++){rename[i]=i;}
  for(int y=startY+1; y<endY; y++){
    for(int x=startX; x<endX; x++){
      if(out[x+(y-1)*width]){
        rename[out[x+y*width]] = std::min(rename[out[x+y*width]], out[x+(y-1)*width]);
      }
    }

    for(int x=startX; x<endX; x++){
      area[out[x+y*width]]--;
      out[x+y*width] = rename[out[x+y*width]];
      area[out[x+y*width]]++;
    }

  }

  // prop y up
  for(int i=0; i<maxLabels; i++){rename[i]=i;}
  for(int y=endY-2; y>=startY; y--){
    for(int x=startX; x<endX; x++){
      if(out[x+(y+1)*width]){
        rename[out[x+y*width]] = std::min(rename[out[x+y*width]], out[x+(y+1)*width]);
      }
    }

    for(int x=startX; x<endX; x++){
      area[out[x+y*width]]--;
      out[x+y*width] = rename[out[x+y*width]];
      area[out[x+y*width]]++;
    }

  }

  // prop x right
  for(int i=0; i<maxLabels; i++){rename[i]=i;}
  for(int x=startX+1; x<endX; x++){
    for(int y=startY; y<endY; y++){
      if(out[(x-1)+y*width]){
        rename[out[x+y*width]] = std::min(rename[out[x+y*width]], out[(x-1)+y*width]);
      }
    }

    for(int y=startY; y<endY; y++){
      area[out[x+y*width]]--;
      out[x+y*width] = rename[out[x+y*width]];
      area[out[x+y*width]]++;
    }
  }

  // prop x left
  for(int i=0; i<maxLabels; i++){rename[i]=i;}
  for(int x=endX-2; x>=startX; x--){
    for(int y=startY; y<endY; y++){
      if(out[(x+1)+y*width]){
        rename[out[x+y*width]] = std::min(rename[out[x+y*width]], out[(x+1)+y*width]);
      }
    }

    for(int y=startY; y<endY; y++){
      area[out[x+y*width]]--;
      out[x+y*width] = rename[out[x+y*width]];
      area[out[x+y*width]]++;
    }
  }

}

void computeArea(
  unsigned short *labels,
  unsigned short area[maxLabels],
  int width,
  int height){
  
  for(int i=0; i<maxLabels; i++){area[i]=0;}

  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){
      area[labels[x+y*width]]++;
    }
  }
}

void label(
  unsigned char *in, 
  unsigned short *out, 
  unsigned short area[maxLabels],
  int width, int height, 
  unsigned short firstId,
  unsigned int minArea,
  int startX, int endX,
  int startY, int endY){

  //  unsigned short area[maxLabels];
  for(int i=0; i<maxLabels; i++){area[i]=0;}

  bool live[maxLabels];
  for(int i=0; i<maxLabels; i++){live[i]=false;}

  // rake X
  unsigned short id = firstId; // atomic

  for(int y=startY; y<endY; y++){
    if(in[startX+y*width]){
      out[startX+y*width]=id; // atomic
      area[id]++; // atomic
      assert(id<maxLabels);
      id++;

      // save liveness
      if(y==startY || y==endY-1){live[id-1]=true;}
    }else{
      out[startX+y*width]=0;
    }
  }

  for(int x=startX+1; x<endX; x++){
    // this is our vector
    for(int y=startY; y<endY; y++){
      if(in[x+y*width] != in[(x-1)+y*width]){
	if(in[x+y*width]){
	  out[x+y*width]=id; // atomic
	  area[id]++; // atomic
	  assert(id<maxLabels);
	  id++;

	  // save liveness
	  if(y==startY || y==endY-1){live[id-1]=true;}

	}else{
	  out[x+y*width]=0;
	}
      }else{
	out[x+y*width] = out[(x-1)+y*width];
	area[out[x+y*width]]++;
      }
    }
  }

  labelProp(out,area,width,height,startX,endX,startY,endY);
  labelProp(out,area,width,height,startX,endX,startY,endY);

  // delete stuff
  for(int x=startX; x<endX; x++){
    // this is our vector
    for(int y=startY; y<endY; y++){
      if(area[out[x+y*width]]<minArea && !live[out[x+y*width]]){
	out[x+y*width]=0;
	in[x+y*width]=0;
      }
    }
  }

  // keep area array accurate
  for(int i=0; i<maxLabels; i++){
    if(area[i]<minArea && !live[i]){area[i]=0;}
  }
}

// returns the largest label
// note: destroys area
unsigned short condenseLabels(
  unsigned short *labels, unsigned short area[maxLabels],
  unsigned short firstId,
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY){

  unsigned short id=firstId;
  area[0]=0;
  for(int i=0; i<maxLabels; i++){
    if(area[i]){
      area[i]=id;
      id++;
    }
  }

  for(int x=startX; x<endX; x++){
    // this is our vector
    for(int y=startY; y<endY; y++){
      labels[x+y*width] = area[labels[x+y*width]];
    }
  }
  
  return (id-1);
}

void invert(unsigned char *in, int width, int height){
  for(int x=1; x<width; x++){
    for(int y=1; y<height; y++){
      in[x+y*width] = (in[x+y*width]>0)?0:255;
    }
  }
}

void computeBB(
  unsigned short *labels,
  unsigned short l[maxLabels], // smallest x
  unsigned short r[maxLabels],  // largest x
  unsigned short t[maxLabels], // the largest y value
  unsigned short b[maxLabels],  // the smallest y value
  int width,
  int height){

  for(int i=0; i<maxLabels; i++){
    l[i]=width;
    r[i]=0;
    t[i]=0;
    b[i]=height;
  }

  for(int x=1; x<width; x++){
    for(int y=1; y<height; y++){
      int id=labels[x+y*width];
      l[id] = std::min(l[id],(unsigned short)x);
      r[id] = std::max(r[id],(unsigned short)x);
      t[id] = std::max(t[id],(unsigned short)y);
      b[id] = std::min(b[id],(unsigned short)y);
    }
  }

}

void filter(
  unsigned char *in, 
  unsigned char *reason,
  unsigned short *labels, 
  int width,
  int height){

  unsigned short area[maxLabels];
  computeArea(labels,area,width,height);

  unsigned short l[maxLabels];
  unsigned short r[maxLabels];
  unsigned short t[maxLabels];
  unsigned short b[maxLabels];
  computeBB(labels,l,r,t,b,width,height);

  bool kill[maxLabels];
  unsigned char kreason[maxLabels];

  for(int i=0; i<maxLabels; i++){
    if(area[i]>0){ // is this region still there?
      
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
      }else if(area[i]<6 || areaPercent>0.2f){
	kill[i] = true;
	kreason[i]=100;
      }else if(solidity < 0.20f){
	kill[i] = true;
	kreason[i]=150;
      }else{
	kill[i]=false;
	kreason[i]=0;
      }
      
    }else{
      kill[i] = false;
      kreason[i]=0;
    }
  }


  for(int x=0; x<width; x++){
    for(int y=0; y<height; y++){
      if(kill[labels[x+y*width]]){
	in[x+y*width] = 0;
      }
      reason[x+y*width]=kreason[labels[x+y*width]];
    }
  }

}

void makePyramid(
  unsigned char *in, unsigned char ***out, int desiredLevels,
  int width, int height){

  // build space
  *out = new unsigned char*[desiredLevels];
  (*out)[0]=in;
  for(int i=1; i<desiredLevels; i++){ (*out)[i]=new unsigned char[width*height/(int)pow(2,2*i)];}

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
	(*out)[L][x+y*lw]=(cnt>2)?255:0;
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
  int vecSizeY=16;
  int vecSizeX=160;

  unsigned char *dist=new unsigned char[width*height];
  unsigned short *labels = new unsigned short[width*height];
  unsigned short area[maxLabels];
  unsigned char *reason = new unsigned char[width*height];

  for(int L = 3; L>=0; L--){
    int lw = width/pow(2,L);
    int lh = height/pow(2,L);

    unsigned short firstId = 1;

    for(int by=0; by<ceil((float)lh/(float)vecSizeY); by++){
      for(int bx=0; bx<ceil((float)lw/(float)vecSizeX); bx++){
	int endY = (by+1)*vecSizeY;
	if(endY>lh){endY=lh;}
	
	int endX = (bx+1)*vecSizeX;
	if(endX>lw){endX=lw;}
	
	label(pyramid[L],labels,area,lw,lh,firstId,6,vecSizeX*bx,endX,vecSizeY*by,endY);
	firstId=condenseLabels(labels,area,firstId,lw,lh,vecSizeX*bx,endX,vecSizeY*by,endY);
	firstId++;
	printf("ML %d\n",(int)firstId);
      }
    }
    
    // fix labels across region boundries
    labelProp(labels,area,lw,lh,0,lw,0,lh);
    computeArea(labels,area,lw,lh);
    int totLabels=condenseLabels(labels,area,1,lw,lh,0,lw,0,lh);
    printf("TL %d\n",totLabels);
    
    distanceTransform(pyramid[L],dist,0,lw,lh);
    
    filter(pyramid[L],reason,labels,lw,lh);

    // now what's left "must" be text, so delete it from other pyrmid levels and save it

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
  }

  saveImage("./res.bmp", width, height, 1, dataGray);

  return 0;
}
