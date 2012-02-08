#ifndef UTIL_H
#define UTIL_H 1

enum Color {RED, GREEN, BLUE, ALPHA, LUM};

const float PI = 3.1415926f;

bool loadImage(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool saveImage(const char *file, int width, int height, int channels, unsigned char *data);

// input data should be [0,1]
bool saveImage(const char *file, int width, int height, int channels, float *data);

unsigned char sampleBilinear(int width, int height, float x, float y, const unsigned char* in);
unsigned char sampleNearest(int width, int height, float x, float y, const unsigned char* in);

void normalizeKernel(int width, int height, float *kernel);
float sum(int width, int height, float *kernel);

void drawLine( int width, int height, unsigned char *data, int x0, int y0, int x1, int y1);

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
