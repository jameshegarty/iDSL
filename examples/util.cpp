#include "util.h"
#include <stdio.h>
#include "string.h"
#include <math.h>
#include <assert.h>

unsigned int endian(unsigned int x){return x;}

void toGrayscale( int width, int height, unsigned int *dataIn, float *grayscaleOut){

  for(int i=0; i<width*height; i++){
    float r = dataIn[i*3];
    float g = dataIn[i*3+1];
    float b = dataIn[i*3+2];
    grayscaleOut[i] = sqrt(r*r+g*g+b*b) / sqrt(3*255*255);
  }

}

void drawLine( 
  int width, 
  int height, 
  unsigned char *data, 
  int x0, 
  int y0, 
  int x1, 
  int y1){

  float dx = abs(x1-x0);
  float dy = abs(y1-y0);

  float sx = -1;
  float sy = -1;
  if(x0 < x1){sx = 1;}
  if(y0 < y1){sy = 1;}
  float err = dx-dy;

  while(1){
    data[(y0*width+x0)*3] = 255;
    data[(y0*width+x0)*3+1] = 0;
    data[(y0*width+x0)*3+2] = 0;

    if(x0 == x1 && y0 == y1){break;}
    float e2 = 2*err;

    if(e2 > -dy){
      err = err - dy;
      x0 = x0 + sx;
    }

    if(e2 <  dx){
      err = err + dx;
      y0 = y0 + sy;
    }
  }
}

void normalizeKernel(int width, int height, float *kernel){
  // normalize
  float sum = 0.f;
  for(int i=0; i<width*height; i++){sum += kernel[i];}
  sum = 1.f / sum;
  for(int i=0; i<width*height; i++){kernel[i] *= sum;}
}

float sum(int width, int height, float *kernel){
  float s = 0.f;

  for(int i=0; i<width*height; i++){
    s += kernel[i];
  }

  return s;
}

unsigned char sampleBilinear(int width, int height, float x, float y, unsigned char* in){
  if(x < 0.f){return 0;}
  if(y < 0.f){return 0;}
  if(x > float(width)){return 0;}
  if(y > float(height)){return 0;}

  int ix = x;
  int iy = y;

  float aa = in[iy*width+ix];
  float ba = in[iy*width+ix+1];
  float ab = in[(iy+1)*width+ix];
  float bb = in[(iy+1)*width+ix+1];
  float rx = x-floor(x);
  float rxi = 1.f-rx;
  float ry = y-floor(y);
  float ryi = 1.f-ry; // 1-ry
  assert(rx >= 0.f);assert(rx <=1.f);assert(rxi >=0.f);assert(rxi <= 1.f);
  float resa = aa*rxi+ba*rx;
  float resb = ab*rxi+bb*rx;

  return (unsigned char)(resa*ryi+resb*ry);
}

unsigned char sampleNearest(int width, int height, float x, float y, unsigned char* in){
  if(x < 0.f){return 0;}
  if(y < 0.f){return 0;}
  if(x > float(width)){return 0;}
  if(y > float(height)){return 0;}

  int ix = x;
  int iy = y;

  return in[iy*width+ix];
}

bool loadImage(const char *filename, int* width, int* height, int *channels, unsigned char **data){

  unsigned int currentCurPos=0;
  
  FILE *file = fopen(filename,"rb");
  fseek(file, 0L, SEEK_END);
  //unsigned long totalfilesize = ftell(file);

  fseek(file, 2, SEEK_SET);
  currentCurPos += 2;

  unsigned int totalsize=0;	//headersize:56+ width*height*bpp
  int i = 0;

  if ((i = fread(&totalsize, sizeof(totalsize), 1, file)) != 1) {
    printf("Error reading compression");
    return false;
  }
  currentCurPos += sizeof(totalsize);

  totalsize=endian(totalsize);
	
  // seek through the bmp header, up to the width/height:
  fseek(file, 4, SEEK_CUR);
  currentCurPos += 4;
  unsigned int headersize=0;	//headersize:56+ width*height*bpp
  
  if ((i = fread(&headersize, sizeof(headersize), 1, file)) != 1) {
    printf("Error reading compression");
    return false;
  }
  currentCurPos += sizeof(headersize);
  headersize=endian(headersize);
  
  fseek(file, 4, SEEK_CUR);
  currentCurPos += 4;

  i = fread(width, 4, 1, file);
  currentCurPos += 4;
  *width = endian(*width);
  
  // read the width
  if (i != 1) {
    printf("Error reading width");
    return false;
  }

  // read the height 
  i = fread(height, 4, 1, file);
  currentCurPos += 4;
  *height = endian(*height);
    
  if (i != 1) {
    printf("Error reading height");
    return false;
  }
    
  // read the planes
  unsigned short planes;          // number of planes in image (must be 1) 
  i=fread(&planes, 2, 1, file);
  currentCurPos += 2;
  planes=endian(planes);
  
  if (i != 1) {
    printf("Error reading planes");
    return false;
  }
    
  if (planes != 1) {
    printf("Planes is %d, not 1 like it should be",planes);
    return false;
  }

  // read the bpp
  unsigned short bpp;             // number of bits per pixel (must be 24)
  if ((i = fread(&bpp, 2, 1, file)) != 1) {
    printf("error reading bpp");
    return false;
  }
  currentCurPos += 2;
  bpp=endian(bpp);

  // read compression
  unsigned int compression=0;

  if ((i = fread(&compression, sizeof(compression), 1, file)) != 1) {
    printf("error reading compression");
    return false;
  }
  currentCurPos += sizeof(compression);
  compression=endian(compression);
  
  // read csize
  unsigned int csize=0;

  if ((i = fread(&csize, sizeof(csize), 1, file)) != 1) {
    printf("error reading csize");
    return false;
  }
  currentCurPos += sizeof(csize);
  csize=endian(csize);
    
  if (bpp == 24) {
    *channels = 3;
  }else if (bpp ==32) {
    *channels = 4;
  }else{
    printf("Bpp from %s is not 24 or 32: %u\n", filename, bpp);
    return false;
  }
  
  unsigned long size = (*width) * (*height) * (*channels);
  
  // seek past the rest of the bitmap header.
  fseek(file, 16, SEEK_CUR);
  currentCurPos += 16;

  // bmps pad each row to be a multiple of 4 bytes
  int padding = 4 - ((*width) * (*channels) % 4);
  padding = padding == 4 ? 0 : padding;

  // allocate array for data
  *data=new unsigned char[size];

  if (*data == NULL) {
    printf("Error allocating memory for color-corrected image data");
    return false;	
  }

  if(padding!=0){
    
    unsigned char *tempData=new unsigned char[size+padding*(*height)];
    
    fseek(file, headersize-currentCurPos, SEEK_CUR);
    
    if ((i = fread(tempData, size+padding*(*height), 1, file)) != 1) {
      printf("Error reading image data");
      delete[] tempData;
      return false;
    }
    
    for(int h=0; h<(*height); h++){  
      memcpy(
	&((*data)[h*(*width)*(*channels)]),
	&tempData[h*(*width)*(*channels)+h*padding],
	(*width)*(*channels));
    }

    delete[] tempData;
  }else{
    fseek(file, headersize-currentCurPos, SEEK_CUR);
    
    if ((i = fread(*data, size, 1, file)) != 1) {
      printf("Error reading image data");
      return false;
    }
  }
  
  unsigned char temp = 0;


  for (unsigned int j=0; j<size; j+=*channels ) { // reverse all of the colors. (bgr -> rgb)
    temp = (*data)[j];
    (*data)[j] = (*data)[j+2];
    (*data)[j+2] = temp;
    
    (*data)[j]=(*data)[j];
    (*data)[j+1]=(*data)[j+1];
    (*data)[j+2]=(*data)[j+2];
  }
  
  fclose(file);

  return true;
}

bool saveImage(const char *filename, int width, int height, int channels, float *data){
  // convert float to unsigned char

  unsigned char *temp = new unsigned char[width*height*channels];

  for(int i=0; i<width*height*channels; i++){
    //    temp[i] = (unsigned char)(data[i]*255.f);
    temp[i] = (unsigned char)( ((data[i]+1.f)/2.f) * 255.f );
  }

  bool res = saveImage(filename,width,height,channels,temp);
  delete[] temp;

  return res;
}

bool saveImage(const char *filename, int width, int height, int channels, unsigned char *data){
  FILE *file;
    
  // make sure the file is there.
  if ((file = fopen(filename, "wb"))==NULL){
    printf("could't open file for writing");
    return false;
  }
    
  unsigned char BMPheader[2]={66,77};

  if ((fwrite(&BMPheader, sizeof(BMPheader), 1, file)) != 1) {
    printf("[Bmp::save] Error writing header");
    return false;
  }
    
  unsigned int totalsize=width * height * 3 +54; // we always write 3 channels
    
  if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
    printf("[Bmp::save] Error writing header");
    return false;
  }

  totalsize=0;
    
  if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
    printf("[Bmp::save] Error writing header");
      
    return false;
      
  }
    
  //offset to data=54
  totalsize=54;
    
  if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
    printf("[Bmp::save] Error writing header");
      
    return false;
      
  }
    
  //	Size of InfoHeader =40 
  totalsize=40;
    
  if ((fwrite(&totalsize, sizeof(totalsize), 1, file)) != 1) {
    printf("[Bmp::save] Error writeing header");
      
    return false;
      
  }
    
  unsigned int owidth=width;
  unsigned int oheight=height;
    
  // write the width
  if ((fwrite(&owidth, 4, 1, file)) != 1) {
    printf("Error writeing width");
      
    return false;
      
  }
    
  // write the height 
  if ((fwrite(&oheight, 4, 1, file)) != 1) {
    printf("Error writing height");
      
    return false;
  }
    
    
  // write the planes
  unsigned short int planes=1;
    
  if ((fwrite(&planes, 2, 1, file)) != 1) {
    printf("Error writeing planes");
      
    return false;
  }
    
  // write the bpp
  unsigned short bitsPP = 24;
    
  if ((fwrite(&bitsPP, 2, 1, file)) != 1) {
    printf("[Bmp save] Error writeing bpp");
    return false;
  }
    
  //compression
  unsigned int comp=0;
    
  if ((fwrite(&comp, 4, 1, file)) != 1) {
    printf("Error writeing planes to ");
    return false;
  }
    
  //imagesize
  comp=0;
    
  if ((fwrite(&comp, 4, 1, file)) != 1) {
    printf("Error writeing planes to ");
    return false;
  }
    
  //XpixelsPerM
  comp=0;
    
  if ((fwrite(&comp, 4, 1, file)) != 1) {
    printf("Error writeing planes to ");
    return false;
  }
    
  //YpixelsPerM
  comp=0;
    
  if ((fwrite(&comp, 4, 1, file)) != 1) {
    printf("Error writeing planes to ");
    return false;
  }
    
  //ColorsUsed
  comp=0;
    
  if ((fwrite(&comp, 4, 1, file)) != 1) {
    printf("Error writeing planes to ");
    return false;
  }
    
  //ColorsImportant
  comp=0;
     
  if ((fwrite(&comp, 4, 1, file)) != 1) {
    printf("Error writeing planes to ");
    return false;
  }
    
  // read the data. 
    
  if (data == NULL) {
    printf("[Bmp save] No image data to write!");
    return false;	
  }
    
  unsigned char temp;

  // bmps pad each row to be a multiple of 4 bytes
  int padding = 4 - (width * 3 % 4); // we always write 3 channels
  padding = padding == 4 ? 0 : padding;
  char pad[]={0,0,0};

  if(channels==3){
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    unsigned long size = height * width * channels;

    for (unsigned int i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
      temp = data[i];
      data[i] = data[i+2];
      data[i+2] = temp;
    }
    
    if(padding != 0){
      for(int h=0; h<height; h++){
	fwrite(&data[h*width*channels], width*channels, 1, file);
	fwrite(pad,padding,1,file);
      }
    }else{
      if ((fwrite(data, size, 1, file)) != 1) {
	printf("[Bmp save] Error writeing image data");
	return false;
      }
    }

    for (unsigned int i=0;i<size;i+=3) { // new we switch them back to RGB.  shoud figure out some way to not have to do this
      temp = data[i];
      data[i] = data[i+2];
      data[i+2] = temp;
    }
      
  }else if(channels==1){
    //we need to expand an alpha into 3 components so taht it can actually be read
    unsigned char* expanded=new unsigned char[width*height*3];
      
    for(int m=0; m<(width*height); m++){
      expanded[(m*3)]=data[m];
      expanded[(m*3)+1]=data[m];
      expanded[(m*3)+2]=data[m];
    }
       
    if(padding != 0){
      for(int h=0; h<height; h++){
	fwrite(&expanded[h*width*3], width*3, 1, file);
	fwrite(pad,padding,1,file);
      }
    }else{
      if ((fwrite(expanded,width*height*3, 1, file)) != 1) {
	printf("[Bmp save] Error writing image data");
	delete[] expanded;
	return false;
      }
    }

    delete[] expanded;
  }else{
    assert(false);
  }
    
    
  fclose(file);
    
  // we're done.
  return true;
}
