#ifndef UTIL_H
#define UTIL_H 1

bool loadImage(const char *file, int* width, int* height, int *channels, unsigned char **data);
bool saveImage(const char *file, int width, int height, int channels, unsigned char *data);

#endif
