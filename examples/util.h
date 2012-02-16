#ifndef UTIL_H
#define UTIL_H 1

enum Color {RED, GREEN, BLUE, ALPHA, LUM};

const float PI = 3.1415926f;

bool loadImage(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool loadImage(const char *file, int* width, int* height, int *channels, unsigned short **data);
bool saveImage(const char *file, int width, int height, int channels, unsigned char *data);
// input data should be [0,1]
bool saveImage(const char *file, int width, int height, int channels, float *data);

bool loadBMP(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool saveBMP(const char *file, int width, int height, int channels, unsigned char *data);
bool saveBMP(const char *file, int width, int height, int channels, float *data);
bool loadPGM(const char *file, int* width, int* height, unsigned short **data); // return 16 bit data
bool loadTMP(const char *file, int* width, int* height, int* channels, unsigned short **data); // return 16 bit data

unsigned char sampleBilinear(int width, int height, float x, float y, const unsigned char* in);
unsigned char sampleNearest(int width, int height, float x, float y, const unsigned char* in);

void normalizeKernel(int width, int height, float *kernel);
float sum(int width, int height, float *kernel);

void drawLine( int width, int height, unsigned char *data, int x0, int y0, int x1, int y1);

void toGrayscale( int width, int height, unsigned char *dataIn, float *grayscaleOut);
void toGrayscale( int width, int height, unsigned char *dataIn, unsigned char *grayscaleOut);

unsigned short mean(int size, unsigned short *in);

template<typename T>
T clamp(T Value, T Min, T Max){
  return (Value < Min)? Min : (Value > Max)? Max : Value;
}


unsigned char readPixel(int imgWidth, int imgHeight, int nChannels, 
                       int x, int y, Color colorChannel,
                       const unsigned char *data);

void writePixel(int imgWidth, int imgHeight, int nChannels, 
                        int x, int y, Color colorChannel,
                        unsigned char *data, unsigned char color);

#endif
