#ifndef __HYPTERM_THREADED_H__
#define __HYPTERM_THREADED_H__

#include <cmath>
#include "constants.h"
#include "hypterm.h"
#include "hypterm_ispc.h"

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
    int blocksizeL1;
    int blocksizeL2;
    int blocksizeL3;
};

void *hypterm_threaded(void *threadarg);
void *hypterm_threaded_ispc(void *threadarg);

#endif
