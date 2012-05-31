const int maxLabels = 1024; // 512
const int maxSerialLabels = 4096;

void labelProp(
  unsigned short *out, 
  unsigned short area[maxLabels],
  int width, int height,
  int startX, int endX,
  int startY, int endY);

void computeArea(
  unsigned short *labels,
  unsigned short area[maxLabels],
  int width,
  int height,
  int startX, int endX,
  int startY, int endY);

void label(
  unsigned char *in, 
  unsigned short *out, 
  unsigned char *reason, 
  unsigned short area[maxLabels],
  int width, int height, 
  unsigned int minArea,
  int startX, int endX,
  int startY, int endY);

bool condenseLabels(
  unsigned short *labels, unsigned short area[maxLabels],
  unsigned short firstId,
  unsigned short *lastId,
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY);

void computeBB(
  unsigned short *labels,
  unsigned short l[maxLabels], // smallest x
  unsigned short r[maxLabels],  // largest x
  unsigned short t[maxLabels], // the largest y value
  unsigned short b[maxLabels],  // the smallest y value
  int width,
  int height,
  int startX, int endX,
  int startY, int endY);


bool checkArea(
  unsigned short *labels, unsigned short area[maxLabels],
  const int width, const int height,
  const int startX, const int endX,
  const int startY, const int endY);

void labelSerial(
  unsigned char *in,
  unsigned short *out,
  int width, int height);
