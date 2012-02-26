#ifndef SVDCMP_H
#define SVDCMP_H 1

void svdcmp(double **a, int m, int n, double w[], double **v);
double **dmatrix(int nrl, int nrh, int ncl, int nch);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);

#endif
