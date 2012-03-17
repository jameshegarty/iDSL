#include "hypterm_threaded.h"

void *hypterm_threaded(void *threadarg){
    struct thread_data *calc_params = (struct thread_data *) threadarg;
    int thread_id = calc_params->thread_id;
    int n[3]={calc_params->n0,calc_params->n1,calc_params->n2};
    int ns[3]={calc_params->n0s,calc_params->n1s,calc_params->n2s};
    int ne[3]={calc_params->n0e,calc_params->n1e,calc_params->n2e};
    int ng[3]={calc_params->ng0,calc_params->ng1,calc_params->ng2};
    double dx[3]={calc_params->dx0,calc_params->dx1,calc_params->dx2};
    int blocksize=calc_params->blocksize;

    int nspec=calc_params->nspec;
    double *pres=calc_params->pres;
    double *cons=calc_params->cons;
    double *flux=calc_params->flux;
    hypterm_serial(n,ns,ne,ng,dx,nspec,cons,pres,flux,blocksize);
}

void *hypterm_threaded_ispc(void *threadarg){
    struct thread_data *calc_params = (struct thread_data *) threadarg;
    int thread_id = calc_params->thread_id;
    int n[3]={calc_params->n0,calc_params->n1,calc_params->n2};
    int ns[3]={calc_params->n0s,calc_params->n1s,calc_params->n2s};
    int ne[3]={calc_params->n0e,calc_params->n1e,calc_params->n2e};
    int ng[3]={calc_params->ng0,calc_params->ng1,calc_params->ng2};
    double dx[3]={calc_params->dx0,calc_params->dx1,calc_params->dx2};
    int blocksize=calc_params->blocksize;

    int nspec=calc_params->nspec;
    double *pres=calc_params->pres;
    double *cons=calc_params->cons;
    double *flux=calc_params->flux;
    ispc::hypterm_ispc(n,ns,ne,ng,dx,nspec,cons,pres,flux,blocksize);
}
