#ifndef UTIL_H
#define UTIL_H 1

const float PI = 3.1415926f;

bool loadImage(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool saveImage(const char *file, int width, int height, int channels, unsigned char *data);

// input data should be [0,1]
bool saveImage(const char *file, int width, int height, int channels, float *data);

unsigned char sampleBilinear(int width, int height, float x, float y, unsigned char* in);
unsigned char sampleNearest(int width, int height, float x, float y, unsigned char* in);

void normalizeKernel(int width, int height, float *kernel);
float sum(int width, int height, float *kernel);

template<typename T>
T clamp(T Value, T Min, T Max){
  return (Value < Min)? Min : (Value > Max)? Max : Value;
}

#endif
