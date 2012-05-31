#include "../label/label.h"
#include <algorithm>
#include <stdio.h>
#include "../util.h"

bool ellipseRay(
  float eX, float eY, float eW, float eH, 
  float ROx, float ROy, float RDx, float RDy){

  return false;
}

void findLines(
  unsigned char *in,
  unsigned short *labels,
  int width,
  int height,
  float minAreaRatio){

  unsigned short area[maxSerialLabels];
  unsigned short l[maxSerialLabels];
  unsigned short r[maxSerialLabels];
  unsigned short t[maxSerialLabels];
  unsigned short b[maxSerialLabels];

  computeArea(labels, area, width, height, 0, width, 0, height);
  unsigned short totalLabels = 0;
  condenseLabels(labels, area, 1,&totalLabels,width,height,0,width,0,height);
  totalLabels++;
  computeArea(labels, area, width, height, 0, width, 0, height);
  computeBB(labels,l,r,t,b,width,height,0,width,0,height);

  unsigned short w[maxSerialLabels];
  unsigned short h[maxSerialLabels];
  unsigned short cx[maxSerialLabels];
  unsigned short cy[maxSerialLabels];

  bool definitelyText[maxSerialLabels]; // everything other than this will be deleted
  for(int i=0;i<maxSerialLabels;i++){definitelyText[i]=false;}

  for(int i=1; i<totalLabels; i++){
    for(int j=i+1; j<totalLabels; j++){

      float ratio = std::min((float)area[i],(float)area[j])/std::max((float)area[i],(float)area[j]);

      if(ratio>minAreaRatio && !definitelyText[i] && !definitelyText[j]){
	// ray from i to j
	int ROx=cx[i];
	int ROy=cy[i];
	int RDx=cx[j]-cx[i];
	int RDy=cy[j]-cy[i];
	
	int matches = 2;

	// find stuff in a line with this ray
	for(int k=0; k<totalLabels;k++){
	  if(ellipseRay(cx[k],cy[k],w[k],h[k],ROx,ROy,RDx,RDy)){
	    float aratio = std::min((float)area[k],(float)area[j])/std::max((float)area[k],(float)area[j]);
	    float bratio = std::min((float)area[k],(float)area[i])/std::max((float)area[k],(float)area[i]);
	    if(aratio<minAreaRatio || bratio<minAreaRatio){
	      matches++;
	      definitelyText[k]=true;
	    }
	  }
	}

	if(matches>3){
	  definitelyText[i]=true;
	  definitelyText[j]=true;
	}

      }
    }
  }

  for(int y=0; y<height; y++){
    for(int x=0; x<width; x++){
      unsigned short lab = labels[x+y*width];
      if(!definitelyText[lab]){in[x+y*width]=0;}
    }
  }
}

int main(int argc, char **argv){

  if(argc!=2){
    printf("Usage: label image.type");
    return 1;
  }

  int width, height, channels;
  unsigned char *data;

  bool res = loadImage(argv[1], &width, &height, &channels, &data);

  unsigned char *dataGray= NULL;

  if(channels==3){
    dataGray = new unsigned char[width*height];
    toGrayscale(width,height,data,dataGray);
  }else if(channels==1){
    dataGray=data;
  }else{
    printf("error wrong # channels\n");
    return 1;
  }

  unsigned short *out = new unsigned short[width*height];

  labelSerial(dataGray,out,width,height);

  res = saveImage("out.bmp", width, height, out);

  delete[] out;
  delete[] data;

  return 0;
}
