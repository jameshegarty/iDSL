//const int maxLabels = 1024; // 512
//const int maxSerialLabels = 4096;

void labelProp(
  unsigned short *out, 
  unsigned short area[],
  int width, int height,
  int startX, int endX,
  int startY, int endY,
  int maxLabels);

void computeArea(
  unsigned short *labels,
  unsigned short area[],
  int width,
  int height,
  int startX, int endX,
  int startY, int endY,
  int maxLabels);

void label(
  unsigned char *in, 
  unsigned short *out, 
  unsigned char *reason, 
  unsigned short area[],
  int width, int height, 
  unsigned int minArea,
  int startX, int endX,
  int startY, int endY,
  int maxLabels);

bool condenseLabels(
  unsigned short *labels, unsigned short area[],
  unsigned short firstId,
  unsigned short *lastId,
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY,
  int maxLabels);

void computeBB(
  unsigned short *labels,
  unsigned short l[], // smallest x
  unsigned short r[],  // largest x
  unsigned short t[], // the largest y value
  unsigned short b[],  // the smallest y value
  int width,
  int height,
  int startX, int endX,
  int startY, int endY,
  int maxLabels);


bool checkArea(
  unsigned short *labels, unsigned short area[],
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY,
  int maxLabels);

void labelSerial(
  unsigned char *in,
  unsigned short *out,
  int width, int height,
  int maxLabels);
