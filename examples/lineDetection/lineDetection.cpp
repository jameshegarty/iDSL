#include "../label/label.h"
#include <algorithm>
#include <stdio.h>
#include "../util.h"

bool ellipseRay(
  float eX, float eY, float eW, float eH, 
  float ROx, float ROy, float RDx, float RDy){

  ROx-=eX;
  ROy-=eY;

  //float abi = (1.f/pow(eW,2))+(1.f/pow(eH,2));
  float A = ((ROx*ROx)/(eW*eW))+((ROy*ROy)/(eH*eH))-1.f;
  float B = ((2.f*ROx*RDx)/(eW*eW))+((2.f*ROy*RDy)/(eH*eH));
  float C = ((RDx*RDx)/(eW*eW))+((RDy*RDy)/(eH*eH));
  
  //printf("%f %f %f\n",A,B,C);

  return B*B>=4.f*A*C;
}

void findLines(
  unsigned char *in,
  unsigned short *labels,
  int width,
  int height,
  float minAreaRatio,
  float charSpacing,
  int minMatches,
  int maxLabels){

  unsigned short area[maxLabels];
  unsigned short l[maxLabels];
  unsigned short r[maxLabels];
  unsigned short t[maxLabels];
  unsigned short b[maxLabels];

  computeArea(labels, area, width, height, 0, width, 0, height,maxLabels);
  unsigned short totalLabels = 0;
  condenseLabels(labels, area, 1,&totalLabels,width,height,0,width,0,height,maxLabels);
  totalLabels++;
  computeArea(labels, area, width, height, 0, width, 0, height,maxLabels);
  computeBB(labels,l,r,t,b,width,height,0,width,0,height,maxLabels);

  unsigned short w[maxLabels];
  unsigned short h[maxLabels];
  float cx[maxLabels];
  float cy[maxLabels];

  for(int i=0;i<totalLabels;i++){
    //printf("%d %d\n",r[i],l[i]);
    assert(r[i]>=l[i]);
    w[i]=r[i]-l[i]+1;
    assert(t[i]>=b[i]);
    h[i]=t[i]-b[i]+1;
    
    cx[i]=l[i]+float(w[i])/2.f;
    cy[i]=b[i]+float(h[i])/2.f;
  }

  bool definitelyText[maxLabels]; // everything other than this will be deleted
  for(int i=0;i<maxLabels;i++){definitelyText[i]=false;}

  for(int i=1; i<totalLabels; i++){
    for(int j=1; j<totalLabels; j++){

      float ratio = 0;

      if(abs(w[i]-w[j]) < abs(h[i]-h[j])){
	ratio = float(abs(w[i]-w[i]))/std::max(float(w[i]),float(w[j]));
      }else{
	ratio = float(abs(h[i]-h[i]))/std::max(float(h[i]),float(h[j]));
      }

      assert(ratio<=1.f);

      float dist = sqrt(pow(cx[j]-cx[i],2)+pow(cy[j]-cy[i],2));
      float minD = std::min(std::max(w[i],h[i]),std::max(w[j],h[j]));

      if(ratio<minAreaRatio  && dist<minD*charSpacing && i!=j){
	int matches = 2;


	// ray from i to j
	int ROx=cx[i];
	int ROy=cy[i];
	int RDx=cx[j]-cx[i];
	int RDy=cy[j]-cy[i];

	// find stuff in a line with this ray
	for(int k=1; k<totalLabels;k++){
	  if(ellipseRay(cx[k],cy[k],w[k],h[k],ROx,ROy,RDx,RDy)){

	    float aratio = std::min((float)area[k],(float)area[j])/std::max((float)area[k],(float)area[j]);
	    float bratio = std::min((float)area[k],(float)area[i])/std::max((float)area[k],(float)area[i]);
	    assert(aratio<=1.f);
	    assert(bratio<=1.f);

	    float adist = sqrt(pow(cx[k]-cx[i],2)+pow(cy[k]-cy[i],2));
	    float aminD = std::min(std::max(w[i],h[i]),std::min(w[k],h[k]));

	    float bdist = sqrt(pow(cx[k]-cx[j],2)+pow(cy[k]-cy[j],2));
	    float bminD = std::min(std::max(w[j],h[j]),std::min(w[k],h[k]));

	    if( (aratio<minAreaRatio || bratio<minAreaRatio) && (adist<aminD*charSpacing || bdist<bminD*charSpacing)){
	      matches++;
	      definitelyText[k]=true;
	    }
	  }
	}
	*/
	if(matches>=minMatches){
	  //printf("%f %f %f %f %f\n",ratio,minAreaRatio,dist,minD,charSpacing);
	  definitelyText[i]=true;
	  definitelyText[j]=true;
	}
	  //definitelyText[i]=true;
	  //definitelyText[j]=true;
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

const int maxLabels = 4096;

int main(int argc, char **argv){

  if(argc!=5){
    printf("Usage: label image.type minRatio*100 charSpacing*10(typically 2-4) matches(>=)\n");
    return 1;
  }

  int minRatio = atoi(argv[2]);
  int charSpacing = atoi(argv[3]);
  int minMatches = atoi(argv[4]);

  bool a = ellipseRay(5,0,1,1,0,0,0,1);
  assert(!a);
  a = ellipseRay(5,0,1,1,0,0,1,0);
  assert(a);
  a = ellipseRay(5,0,1,1,0,0,5,1);
  assert(a);
  a = ellipseRay(5,0,1,1,0,0,5,1.1f);
  assert(!a);
  a = ellipseRay(5,5,10.f,1.f,0,0,0,1.1f);
  assert(a);
  
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

  labelSerial(dataGray,out,width,height,maxLabels);
  
  findLines(dataGray,out,width,height,float(minRatio)*0.01f,float(charSpacing)*0.1f,minMatches,maxLabels);

  res = saveImage("out.bmp", width, height, 1, dataGray);
  res = saveImage("lab.bmp", width, height, out);

  delete[] out;
  delete[] data;

  return 0;
}
