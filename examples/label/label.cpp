#include "label.h"
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../util.h"

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
  int height,
  int startX, int endX,
  int startY, int endY){
  
  for(int i=0; i<maxLabels; i++){area[i]=0;}

  for(int x=startX; x<endX; x++){
    for(int y=startY; y<endY; y++){
      assert(labels[x+y*width]<maxLabels);
      area[labels[x+y*width]]++;
    }
  }
}

void label(
  unsigned char *in, 
  unsigned short *out, 
  unsigned char *reason, 
  unsigned short area[maxLabels],
  int width, int height, 
  unsigned int minArea,
  int startX, int endX,
  int startY, int endY){

  //  unsigned short area[maxLabels];
  for(int i=0; i<maxLabels; i++){area[i]=0;}


  // rake X
  unsigned short id = 1; // atomic

  for(int y=startY; y<endY; y++){
    if(in[startX+y*width]){
      out[startX+y*width]=id; // atomic
      area[id]++; // atomic
      assert(id<maxLabels);
      id++;
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
	}else{
	  out[x+y*width]=0;
	}
      }else{
	out[x+y*width] = out[(x-1)+y*width];
	area[out[x+y*width]]++;
      }
    }
  }

  assert(checkArea(out,area,width,height,startX,endX,startY,endY));

  labelProp(out,area,width,height,startX,endX,startY,endY);
  labelProp(out,area,width,height,startX,endX,startY,endY);

  assert(checkArea(out,area,width,height,startX,endX,startY,endY));

  bool live[maxLabels];
  for(int i=0; i<maxLabels; i++){live[i]=false;}

  // stuff in first/last column is live
  for(int y=startY; y<endY; y++){
    if(in[(endX-1)+y*width]){ live[out[(endX-1)+y*width]]=true; }
    if(in[startX+y*width]){ live[out[startX+y*width]]=true; }
  }

  for(int x=startX; x<endX; x++){
    if(in[x+startY*width]){ live[out[x+startY*width]]=true; }
    if(in[x+(endY-1)*width]){ live[out[x+(endY-1)*width]]=true; }
  }

  // delete stuff
  for(int x=startX; x<endX; x++){
    // this is our vector
    for(int y=startY; y<endY; y++){
      if(area[out[x+y*width]]<minArea && !live[out[x+y*width]]){
	out[x+y*width]=0;
	in[x+y*width]=0;
	reason[x+y*width]=250;
      }
    }
  }

  // keep area array accurate
  for(int i=0; i<maxLabels; i++){
    if(area[i]<minArea && !live[i]){area[i]=0;}
  }
}

bool checkArea(
  unsigned short *labels, unsigned short area[maxLabels],
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY){

  unsigned short newArea[maxLabels];

  for(int i=0; i<maxLabels; i++){ newArea[i]=0; }

  for(int x=startX; x<endX; x++){
    for(int y=startY; y<endY; y++){
      newArea[labels[x+y*width]]++;
    }
  }

  // don't care about id 0
  for(int i=1; i<maxLabels; i++){
    if(area[i]!=newArea[i]){
      printf("error area %d is %d but should be %d\n",i,area[i],newArea[i]);
      return false;
    }
  }

  return true;
}

// returns the largest label
// note: destroys area array
bool condenseLabels(
  unsigned short *labels, unsigned short area[maxLabels],
  unsigned short firstId,
  unsigned short* lastId,
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY){

  assert(checkArea(labels,area,width,height,startX,endX,startY,endY));

  unsigned short id=firstId;
  area[0]=0;
  for(int i=0; i<maxLabels; i++){
    if(area[i]){
      area[i]=id;
      //assert(id<maxLabels);
      if(id>=maxLabels){return false;} // throw an exception
      id++;
    }
  }

  for(int x=startX; x<endX; x++){
    // this is our vector
    for(int y=startY; y<endY; y++){
      labels[x+y*width] = area[labels[x+y*width]];
    }
  }
  
  *lastId = id-1;
  return true;
}

void computeBB(
  unsigned short *labels,
  unsigned short l[maxLabels], // smallest x
  unsigned short r[maxLabels],  // largest x
  unsigned short t[maxLabels], // the largest y value
  unsigned short b[maxLabels],  // the smallest y value
  int width,
  int height,
  int startX, int endX,
  int startY, int endY){

  for(int i=0; i<maxLabels; i++){
    l[i]=width;
    r[i]=0;
    t[i]=0;
    b[i]=height;
  }

  for(int x=startX; x<endX; x++){
    for(int y=startY; y<endY; y++){
      int id=labels[x+y*width];
      l[id] = std::min(l[id],(unsigned short)x);
      r[id] = std::max(r[id],(unsigned short)x);
      t[id] = std::max(t[id],(unsigned short)y);
      b[id] = std::min(b[id],(unsigned short)y);
    }
  }

}



void labelSerial(
  unsigned char *in,
  unsigned short *out,
  int width, int height){

  unsigned short rename[maxSerialLabels];
  for(int i=0; i<maxSerialLabels; i++){rename[i]=i;}

  int id=1;

  if(in[0]){out[0]=id;id++;}else{out[0]=0;}

  for(int x=1; x<width; x++){
    if(in[x]==0){
      out[x]=0;
    }else if(in[x-1]!=in[x]){
      out[x]=id;
      id++;
    }else{
      out[x]=out[x-1];
    }
  }

  for(int y=1; y<height; y++){
    if(in[y*width]==0){
      out[y*width]=0;
    }else if(in[(y-1)*width]!=in[y*width]){
      out[y*width]=id;
      id++;
    }else{
      out[y*width]=out[(y-1)*width];
    }
  }

  for(int y=1; y<height; y++){
    for(int x=1; x<width; x++){

      if(in[x+y*width]==0){
	out[x+y*width]=0;
      }else if( !in[(x-1)+y*width] && !in[x+(y-1)*width] ){
	out[x+y*width]=id;
	id++;
      }else if(in[(x-1)+y*width] && in[x+(y-1)*width]){
	// get to the bottom of this!
	unsigned short owestLabel = out[(x-1)+y*width];
	unsigned short westLabel = owestLabel;
	unsigned short onorthLabel = out[x+(y-1)*width];
	unsigned short northLabel = onorthLabel;

	if(westLabel!=northLabel){
	  for(int i=0; i<id; i++){
	    //	    printf("%d : %d\n",i,rename[i]);
	  }
	}

	while(rename[westLabel]!=westLabel){westLabel = rename[westLabel];}
	while(rename[northLabel]!=northLabel){northLabel = rename[northLabel];}

	unsigned short newLabel = std::min(westLabel,northLabel);
	if(westLabel!=northLabel){
	  //	  printf("%d %d %d %d %d\n",owestLabel,westLabel,onorthLabel,northLabel,newLabel);
	}
	assert(newLabel>0);

	out[x+y*width]=newLabel;
	rename[westLabel]=newLabel;
	rename[northLabel]=newLabel;

      }else if( !in[(x-1)+y*width] && in[x+(y-1)*width]){
	out[x+y*width] = out[x+(y-1)*width];
      }else if(in[(x-1)+y*width]){
	out[x+y*width]=out[(x-1)+y*width];
      }else{
	printf("%d %d %d %d %d\n",in[(x-1)+y*width],in[x+(y-1)*width],in[x+y*width],out[(x-1)+y*width],out[x+(y-1)*width]);
	assert(false);
      }
    }
  }

  for(int i=0; i<maxSerialLabels;i++){
    int lab = rename[i];
    while(rename[lab]!=lab){lab = rename[lab];}
    rename[i]=lab;
  }

  for(int y=1; y<height; y++){
    for(int x=1; x<width; x++){
      out[x+y*width]=rename[out[x+y*width]];
    }
  }
}
