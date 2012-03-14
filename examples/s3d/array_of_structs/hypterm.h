#ifndef __HYPTERM_H__
#define __HYPTERM_H__

#include <cmath>
#include <algorithm>
#include "constants.h"

void hypterm_serial(int *n,int *ns,int *ne,int *ng,double *dx,int nspec,double *cons,double *pres,double *flux,int blocksize);

#endif
