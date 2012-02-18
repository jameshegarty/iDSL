#ifndef __HYPTERM_H__
#define __HYPTERM_H__

#include <cmath>
#include "constants.h"

struct thread_data{
    int thread_id;
    int n0;
    int n1;
    int n2;
    int n0s;
    int n1s;
    int n2s;
    int n0e;
    int n1e;
    int n2e;
    int ng0;
    int ng1;
    int ng2;
    double dx0;
    double dx1;
    double dx2;
    int nspec;
    double *pres;
    double *cons;
    double *flux;
    int blocksize;
};

void hypterm_serial(int *n,int *ns,int *ne,int *ng,double *dx,int nspec,double *cons,double *pres,double *flux,int blocksize);
void *hypterm_threaded(void *threadarg);

#endif
