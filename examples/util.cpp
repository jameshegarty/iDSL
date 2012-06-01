#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <math.h>
#include <assert.h>

unsigned int endian(unsigned int x);
unsigned int endian(unsigned int x){return x;}
unsigned short endianBig(unsigned short x){return (x>>8) |  (x<<8);} // on intel mac, swap big to little

unsigned short mean(int size, unsigned short *in){
  /*  float mean = in[0];

  for(int i=1; i<size; i++){
    mean = mean + (float(in[0]) - mean)/float(size);
    //if(i%100==0){printf("%f\n",mean);}
  }
  */

  float mean = 0;
  for(int i=0; i<size; i++){
    mean += in[i];
  }

  return mean/float(size);

  return mean;
}

void toGrayscale( int width, int height, unsigned char *dataIn, float *grayscaleOut){

  for(int i=0; i<width*height; i++){
    float r = dataIn[i*3];
    float g = dataIn[i*3+1];
    float b = dataIn[i*3+2];
    grayscaleOut[i] = sqrt(r*r+g*g+b*b) / sqrt(3*255*255);
  }

}

void toGrayscale( int width, int height, unsigned char *dataIn, unsigned char *grayscaleOut){

  for(int i=0; i<width*height; i++){
    float r = dataIn[i*3];
    float g = dataIn[i*3+1];
    float b = dataIn[i*3+2];
    grayscaleOut[i] = sqrt(r*r+g*g+b*b)*255.f/sqrt(255*255*3);
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

unsigned char readPixel(int imgWidth, int imgHeight, int nChannels, 
                        int x, int y, Color colorChannel,
                        const unsigned char *data) {
    
    int byteWidth = imgWidth * nChannels;
    int rowOffset = x * nChannels + colorChannel;
    return data[y * byteWidth + rowOffset];
}

void writePixel(int imgWidth, int imgHeight, int nChannels, 
                int x, int y, Color colorChannel,
                unsigned char *data, unsigned char color) {
    int byteWidth = imgWidth * nChannels;
    int rowOffset = x * nChannels + colorChannel;
    data[y * byteWidth + rowOffset] = color;
}


bool loadImage(const char *filename, int* width, int* height, int *channels, unsigned char **data){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"bmp")==0){
    return loadBMP(filename,width,height,channels,data);
  }else if(strcmp(ext,"ppm")==0){
    return loadPPM(filename,width,height,channels,data);
  }

  printf("unknown filetype %s\n",filename);
  return false;
}

bool checkFloatImage(const char *filename){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"flo")==0){
    return true;
  }

  return false;
}

bool loadImage(const char *filename, int* width, int* height, int *channels, float **data){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"flo")==0){
    *channels = 2;
    return loadFLO(filename,width,height,data);
  }

  printf("unknown filetype %s\n",filename);
  return false;
}

bool loadBMP(const char *filename, int* width, int* height, int *channels, unsigned char **data){

  unsigned int currentCurPos=0;
  
  FILE *file = fopen(filename,"rb");
  fseek(file, 0L, SEEK_END);
  unsigned long totalfilesize = ftell(file);

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

  if ((i = fread(width, 4, 1, file)) != 1) {
    printf("Error reading file\n");
    return false;
  }

  currentCurPos += 4;
  *width = endian(*width);
  
  // read the width
  if (i != 1) {
    printf("Error reading width");
    return false;
  }

  // read the height 
  if ((i = fread(height, 4, 1, file)) != 1) {
    printf("Error reading file\n");
    return false;
  }

  currentCurPos += 4;
  *height = endian(*height);
    
  if (i != 1) {
    printf("Error reading height");
    return false;
  }
    
  // read the planes
  unsigned short planes;          // number of planes in image (must be 1) 
  if ((i=fread(&planes, 2, 1, file)) != 1) {
    printf("Error reading file\n");
    return false;
  }

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

  } else if (bpp ==32) {
    *channels = 4;

    // greyscale images:
  } else if (bpp == 8) {
    *channels = 1;

  } else {
    printf("Bpp from %s is not 8, 24, or 32: %u\n", filename, bpp);
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
      printf("Error reading image data %d %d %d %d\n",padding, *width,*height,headersize);
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
      printf("Error reading image data %d %d %ld %d %d %d %ld %d\n",headersize,currentCurPos,size,*width,*height, *channels,totalfilesize,totalsize);
      return false;
    }
  }
  
  unsigned char temp = 0;

  if (*channels > 2) {
    for (unsigned int j=0; j<size; j+=*channels ) { // reverse all of the colors. (bgr -> rgb)
      temp = (*data)[j];
      (*data)[j] = (*data)[j+2];
      (*data)[j+2] = temp;
    
      (*data)[j]=(*data)[j];
      (*data)[j+1]=(*data)[j+1];
      (*data)[j+2]=(*data)[j+2];
    }
  }
  
  fclose(file);

  return true;
}

bool saveImage(const char *filename, int width, int height, int channels, float *data){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"bmp")==0){
    return saveBMP(filename,width,height,channels,data);
  }else if(strcmp(ext,"flo")==0){
    assert(channels==2);
    return saveFLO(filename,width,height,data);
  }

  return false;
}

bool saveImage(const char *filename, int width, int height, int channels, unsigned char *data){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"bmp")==0){
    return saveBMP(filename,width,height,channels,data);
  }

  return false;
}

bool saveBMP(const char *filename, int width, int height, int channels, float *data){
  // convert float to unsigned char

  unsigned char *temp = new unsigned char[width*height*channels];

  for(int i=0; i<width*height*channels; i++){
    temp[i] = (unsigned char)( ((data[i]+1.f)/2.f) * 255.f );
  }

  bool res = saveBMP(filename,width,height,channels,temp);
  delete[] temp;

  return res;
}

bool saveImageAutoLevels(const char *filename, int width, int height, int channels, float *data){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"bmp")==0){
    return saveBMPAutoLevels(filename,width,height,channels,data);
  }

  return false;
}



bool saveImage(const char *filename, int width, int height, unsigned short *data){
  unsigned char *temp = new unsigned char[width*height*3];

  //  for(int i=0; i<width*height*channels; i++){
  //    temp[i]=data[i] % 256;
  //  }
  for(int y=0; y<height; y++){
    for(int x=0; x<width; x++){
      temp[(x+y*width)*3+0]=data[x+y*width] >> 8;
      temp[(x+y*width)*3+1]=data[x+y*width] & 255;
      temp[(x+y*width)*3+2]=0;


      if(data[x+y*width]>256){
	assert(temp[(x+y*width)*3+0]>0);
      }
    }
  }

  bool res = saveImage(filename,width,height,3,temp);

  delete[] temp;

  return res;
}

bool saveBMPAutoLevels(const char *filename, int width, int height, int channels, float *data){
  // convert float to unsigned char

  unsigned char *temp = new unsigned char[width*height*channels];

  float max = -1000000.f;
  float min = 1000000.f;

  for(int i=0; i<width*height*channels; i++){
    if(data[i]<min){min=data[i];}
    if(data[i]>max){max=data[i];}
  }

  for(int i=0; i<width*height*channels; i++){
    temp[i] = (unsigned char)( ((data[i]-min)/(max-min)) * 255.f );
  }

  bool res = saveBMP(filename,width,height,channels,temp);
  delete[] temp;

  return res;
}

bool saveBMP(const char *filename, int width, int height, int channels, unsigned char *data){
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

bool loadImage(const char *filename, int* width, int* height, int* channels, unsigned short **data){
  const char *ext = filename + strlen(filename) - 3;

  if(strcmp(ext,"pgm")==0){
    *channels = 1;
    return loadPGM(filename,width,height,data);
  }else if(strcmp(ext,"tmp")==0){
    return loadTMP(filename,width,height,channels,data);
  }

  printf("unknown filetype %s %s\n",filename, ext);
  return false;
}

bool loadPGM(const char *filename, int* width, int* height, unsigned short **data){

  assert(sizeof(unsigned short)==2);

  FILE *file = fopen(filename,"rb");

  char temp[16];

  if(fgets(temp,sizeof(temp),file)==NULL){
    printf("Error reading\n");
    return false;
  }

  if(strcmp(temp,"P5\n")!=0){
    printf("Error, incorrect PGM type %s\n",temp);
    return false;
  }

  if(!fgets(temp,sizeof(temp),file)){
    printf("Error reading\n");
    return false;
  }

  printf(" %s\n",temp);

  sscanf(temp,"%d %d", width, height);

  if(!fgets(temp,sizeof(temp),file)){
    printf("Error reading\n");
    return false;
  }

  if(strcmp(temp,"65535\n")==0){
    printf("16 bit");
  }else if(strcmp(temp,"255\n")==0){
    printf("8 bit");
  }else{
    printf("unknown bit depth %s\n",temp);
    return false;
  }

  *data = new unsigned short[ (*width) * (*height)];
  unsigned short *tempData = new unsigned short[ (*width) * (*height)];

  int i = 0;
  if ((i = fread(tempData, (*width)*(*height)*sizeof(unsigned short), 1, file)) != 1) {
    printf("Error reading file\n");
    return false;
  }

  // flip endian + row order
  for(int x=0; x<(*width); x++){
    for(int y=0; y<(*height); y++){
      (*data)[((*height)-y-1)*(*width)+x] = endianBig(tempData[y*(*width)+x]);
    }
  }

  delete[] tempData;

  return true;
}


bool loadPPM(const char *filename, int* width, int* height, int* channels, unsigned char **data){

  FILE *file = fopen(filename,"rb");

  char temp[16];

  // read in type
  if(!fgets(temp,sizeof(temp),file)){ // read in up to a newline
    printf("Error reading\n");
    return false;
  }

  if(strcmp(temp,"P6\n")!=0){
    printf("Error, incorrect PPM type %s\n",temp);
    return false;
  }

  *channels = 3; // always 3 for this type

  // read in width / height
  if(!fgets(temp,sizeof(temp),file)){
    printf("Error reading\n");
    return false;
  }

  printf("%s\n",temp);

  sscanf(temp,"%d %d\n", width, height);

  // read in number of colors
  if(!fgets(temp,sizeof(temp),file)){
    printf("Error reading\n");
    return false;
  }

  if(strcmp(temp,"65535\n")==0){
    printf("error, this function doesn't support 16 bit\n");
    return false;
  }else if(strcmp(temp,"255\n")==0){
    printf("8 bit\n");
  }else{
    printf("unknown bit depth %s\n",temp);
    return false;
  }
  
  // read in data
  *data = new unsigned char[ (*width) * (*height) * (*channels)];
  unsigned char *tempData = new unsigned char[ (*width) * (*height) * (*channels)];

  int i = 0;
  if ((i = fread(tempData, (*width)*(*height)*(*channels)*sizeof(unsigned char), 1, file)) != 1) {
    printf("Error reading file\n");
    return false;
  }

  // flip row order
  for(int c=0; c<(*channels); c++){
    for(int x=0; x<(*width); x++){ 
      for(int y=0; y<(*height); y++){
	(*data)[(((*height)-y-1)*(*width)+x)*3+c] = tempData[(y*(*width)+x)*3+c];
      }
    }
  }

  delete[] tempData;

  return true;
}

bool loadTMP(const char *filename, int* width, int* height, int* channels, unsigned short **data){

  assert(sizeof(unsigned short)==2);

  FILE *file = fopen(filename,"rb");

  enum TypeCode {FLOAT32 = 0, FLOAT64, UINT8, INT8, UINT16, INT16, UINT32, INT32, UINT64, INT64};

  // get the dimensions
  struct header_t {
    int frames, width, height, channels, typeCode;
  } h;

  h.frames = 0;
  h.width = 0;
  h.height = 0;
  h.channels = 0;
  h.typeCode = 0;

  assert(fread(&h, sizeof(int), 5, file) == 5);
  //	 "File ended before end of header\n");

  printf("%d %d %d %d %d\n",h.frames, h.width, h.height, h.channels, h.typeCode);

  assert(h.frames == 1);

  *width = h.width;
  *height = h.height;
  *channels = h.channels;

  if(h.typeCode == FLOAT32){
    printf("float32\n");
  }else{
    printf("unsupported bit depth %d\n",h.typeCode);
    return false;
  }

  int size = (*width) * (*height) * (*channels);
  *data = new unsigned short[size];
  float *tempData = new float[size];

  int i = 0;
  if ((i = fread(tempData, size*sizeof(float), 1, file)) != 1) {
    printf("Error reading file\n");
    return false;
  }

  // convert to short & flip y
  for(int x=0; x<(*width); x++){
    for(int y=0; y<(*height); y++){
      (*data)[((*height)-y-1)*(*width)+x] = tempData[y*(*width)+x];
    }
  }

  delete[] tempData;

  return true;
}

#define TAG_FLOAT 202021.25  // check for this when READING the file
#define TAG_STRING "PIEH"    // use this when WRITING the file

bool saveFLO(const char *filename, int width, int height, float *data){
  FILE *stream = fopen(filename, "wb");
  assert(stream);//, "Could not open %s", filename.c_str());
        
  // write the header
  fprintf(stream, TAG_STRING);
  if( (int)fwrite(&width,  sizeof(int), 1, stream) != 1 ){
    assert(false);
  }
  
  if( (int)fwrite(&height, sizeof(int), 1, stream) != 1 ){
    assert(false);//panic("problem writing header: %s", filename.c_str());
  }

  float *dataTemp = new float[width*height*2];

  // flip row order
  for(int x=0; x<width;x++){
    for(int y=0; y<height; y++){
      dataTemp[((height-y-1)*width+x)*2+0] = data[(y*width+x)*2+0];
      dataTemp[((height-y-1)*width+x)*2+1] = data[(y*width+x)*2+1];
    }
  }

  fwrite(dataTemp, sizeof(float), width * height * 2, stream);
  fclose(stream);

  delete[] dataTemp;

  return true;
}

bool loadFLO(const char *filename, int* width, int* height, float **data){
  FILE *stream = fopen(filename, "rb");
  assert(stream);//, "Could not open file %s\n", filename.c_str());

  float tag;

  if ((int)fread(&tag,    sizeof(float), 1, stream) != 1 ){
    assert(false);
  }

  if( (int)fread(width,  sizeof(int),   1, stream) != 1 ){
    assert(false);
  }

  if( (int)fread(height, sizeof(int),   1, stream) != 1){
    assert(false);//panic("ReadFlowFile: problem reading file %s", filename.c_str());
  }

  assert(tag == TAG_FLOAT);//, "Wrong tag (possibly due to big-endian machine?)");

  // another sanity check to see that integers were read correctly (999999 should do the trick...)
  assert(*width > 0 && *width < 999999);//, "illegal width %d", *width);
  assert(*height > 0 && *height < 999999);//, "illegal height %d", *height);
        
  float *dataTemp = new float[(*width)*(*height)*2];

  size_t n = (*width)*(*height)*2;
  size_t lol = fread(dataTemp, sizeof(float), n, stream);// == n);//,"Unexpected end of file\n");

  if(lol!=n){
    printf("error reading");
    return false;
  }

  fclose(stream);


  *data = new float[(*width)*(*height)*2];

  // flip row order
  for(int x=0; x<(*width);x++){
    for(int y=0; y<(*height); y++){
      (*data)[(((*height)-y-1)*(*width)+x)*2+0] = dataTemp[(y*(*width)+x)*2+0];
      (*data)[(((*height)-y-1)*(*width)+x)*2+1] = dataTemp[(y*(*width)+x)*2+1];
    }
  }

  delete[] dataTemp;

  return true;
}
