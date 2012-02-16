#include "hypterm.h"

void hypterm_serial(const int *n,const int *ng,const double *dx,const int nspec,double ****cons,double ***pres,double ****flux){
    for(int i=ng[0];i<n[0]+ng[0];i++){
        for(int j=ng[1];j<n[1]+ng[1];j++){
            for(int k=ng[2];k<n[2]+ng[2];k++){
                double unp1=cons[i+1][j][k][I_MX]/cons[i+1][j][k][I_RHO];
                double unp2=cons[i+2][j][k][I_MX]/cons[i+2][j][k][I_RHO];
                double unp3=cons[i+3][j][k][I_MX]/cons[i+3][j][k][I_RHO];
                double unp4=cons[i+4][j][k][I_MX]/cons[i+4][j][k][I_RHO];

                double unm1=cons[i-1][j][k][I_MX]/cons[i-1][j][k][I_RHO];
                double unm2=cons[i-2][j][k][I_MX]/cons[i-2][j][k][I_RHO];
                double unm3=cons[i-3][j][k][I_MX]/cons[i-3][j][k][I_RHO];
                double unm4=cons[i-4][j][k][I_MX]/cons[i-4][j][k][I_RHO];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
                (ALPHA*(cons[i+1][j][k][I_MX]-cons[i-1][j][k][I_MX])
                +BETA *(cons[i+2][j][k][I_MX]-cons[i-2][j][k][I_MX])
                +GAMMA*(cons[i+3][j][k][I_MX]-cons[i-3][j][k][I_MX])
                +DELTA*(cons[i+4][j][k][I_MX]-cons[i-4][j][k][I_MX]))/dx[0];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
                (ALPHA*(cons[i+1][j][k][I_MX]*unp1-cons[i-1][j][k][I_MX]*unm1
                      +(pres[i+1][j][k]-pres[i-1][j][k]))
                +BETA *(cons[i+2][j][k][I_MX]*unp2-cons[i-2][j][k][I_MX]*unm2
                      +(pres[i+2][j][k]-pres[i-2][j][k]))
                +GAMMA*(cons[i+3][j][k][I_MX]*unp3-cons[i-3][j][k][I_MX]*unm3
                      +(pres[i+3][j][k]-pres[i-3][j][k]))
                +DELTA*(cons[i+4][j][k][I_MX]*unp4-cons[i-4][j][k][I_MX]*unm4
                      +(pres[i+4][j][k]-pres[i-4][j][k])))/dx[0];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
                (ALPHA*(cons[i+1][j][k][I_MY]*unp1-cons[i-1][j][k][I_MY]*unm1)
                +BETA *(cons[i+2][j][k][I_MY]*unp2-cons[i-2][j][k][I_MY]*unm2)
                +GAMMA*(cons[i+3][j][k][I_MY]*unp3-cons[i-3][j][k][I_MY]*unm3)
                +DELTA*(cons[i+4][j][k][I_MY]*unp4-cons[i-4][j][k][I_MY]*unm4))/dx[0];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
                (ALPHA*(cons[i+1][j][k][I_MZ]*unp1-cons[i-1][j][k][I_MZ]*unm1)
                +BETA *(cons[i+2][j][k][I_MZ]*unp2-cons[i-2][j][k][I_MZ]*unm2)
                +GAMMA*(cons[i+3][j][k][I_MZ]*unp3-cons[i-3][j][k][I_MZ]*unm3)
                +DELTA*(cons[i+4][j][k][I_MZ]*unp4-cons[i-4][j][k][I_MZ]*unm4))/dx[0];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
                (ALPHA*(cons[i+1][j][k][I_ENE]*unp1-cons[i-1][j][k][I_ENE]*unm1
                   +(pres[i+1][j][k]*unp1-pres[i-1][j][k]*unm1))
                +BETA *(cons[i+2][j][k][I_ENE]*unp2-cons[i-2][j][k][I_ENE]*unm2
                   +(pres[i+2][j][k]*unp2-pres[i-2][j][k]*unm2))
                +GAMMA*(cons[i+3][j][k][I_ENE]*unp3-cons[i-3][j][k][I_ENE]*unm3
                   +(pres[i+3][j][k]*unp3-pres[i-3][j][k]*unm3))
                +DELTA*(cons[i+4][j][k][I_ENE]*unp4-cons[i-4][j][k][I_ENE]*unm4
                   +(pres[i+4][j][k]*unp4-pres[i-4][j][k]*unm4)))/dx[0];
                
                for(int nsp=0;nsp<nspec;nsp++){
                    flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
                    (ALPHA*(cons[i+1][j][k][I_SP+nsp]*unp1-cons[i-1][j][k][I_SP+nsp]*unm1)
                    +BETA *(cons[i+2][j][k][I_SP+nsp]*unp2-cons[i-2][j][k][I_SP+nsp]*unm2)
                    +GAMMA*(cons[i+3][j][k][I_SP+nsp]*unp3-cons[i-3][j][k][I_SP+nsp]*unm3)
                    +DELTA*(cons[i+4][j][k][I_SP+nsp]*unp4-cons[i-4][j][k][I_SP+nsp]*unm4))/dx[0];
                }

                unp1 = cons[i][j+1][k][I_MY]/cons[i][j+1][k][I_RHO];
                unp2 = cons[i][j+2][k][I_MY]/cons[i][j+2][k][I_RHO];
                unp3 = cons[i][j+3][k][I_MY]/cons[i][j+3][k][I_RHO];
                unp4 = cons[i][j+4][k][I_MY]/cons[i][j+4][k][I_RHO];
                
                unm1 = cons[i][j-1][k][I_MY]/cons[i][j-1][k][I_RHO];
                unm2 = cons[i][j-2][k][I_MY]/cons[i][j-2][k][I_RHO];
                unm3 = cons[i][j-3][k][I_MY]/cons[i][j-3][k][I_RHO];
                unm4 = cons[i][j-4][k][I_MY]/cons[i][j-4][k][I_RHO];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
                   (ALPHA*(cons[i][j+1][k][I_MY]-cons[i][j-1][k][I_MY])
                   +BETA *(cons[i][j+2][k][I_MY]-cons[i][j-2][k][I_MY])
                   +GAMMA*(cons[i][j+3][k][I_MY]-cons[i][j-3][k][I_MY])
                   +DELTA*(cons[i][j+4][k][I_MY]-cons[i][j-4][k][I_MY]))/dx[1];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
                   (ALPHA*(cons[i][j+1][k][I_MX]*unp1-cons[i][j-1][k][I_MX]*unm1)
                   +BETA *(cons[i][j+2][k][I_MX]*unp2-cons[i][j-2][k][I_MX]*unm2)
                   +GAMMA*(cons[i][j+3][k][I_MX]*unp3-cons[i][j-3][k][I_MX]*unm3)
                   +DELTA*(cons[i][j+4][k][I_MX]*unp4-cons[i][j-4][k][I_MX]*unm4))/dx[1];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
                   (ALPHA*(cons[i][j+1][k][I_MY]*unp1-cons[i][j-1][k][I_MY]*unm1
                      +(pres[i][j+1][k]-pres[i][j-1][k]))
                   +BETA *(cons[i][j+2][k][I_MY]*unp2-cons[i][j-2][k][I_MY]*unm2
                      +(pres[i][j+2][k]-pres[i][j-2][k]))
                   +GAMMA*(cons[i][j+3][k][I_MY]*unp3-cons[i][j-3][k][I_MY]*unm3
                      +(pres[i][j+3][k]-pres[i][j-3][k]))
                   +DELTA*(cons[i][j+4][k][I_MY]*unp4-cons[i][j-4][k][I_MY]*unm4
                      +(pres[i][j+4][k]-pres[i][j-4][k])))/dx[1];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
                   (ALPHA*(cons[i][j+1][k][I_MZ]*unp1-cons[i][j-1][k][I_MZ]*unm1)
                   +BETA *(cons[i][j+2][k][I_MZ]*unp2-cons[i][j-2][k][I_MZ]*unm2)
                   +GAMMA*(cons[i][j+3][k][I_MZ]*unp3-cons[i][j-3][k][I_MZ]*unm3)
                   +DELTA*(cons[i][j+4][k][I_MZ]*unp4-cons[i][j-4][k][I_MZ]*unm4))/dx[1];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
                   (ALPHA*(cons[i][j+1][k][I_ENE]*unp1-cons[i][j-1][k][I_ENE]*unm1
                      +(pres[i][j+1][k]*unp1-pres[i][j-1][k]*unm1))
                   +BETA *(cons[i][j+2][k][I_ENE]*unp2-cons[i][j-2][k][I_ENE]*unm2
                      +(pres[i][j+2][k]*unp2-pres[i][j-2][k]*unm2))
                   +GAMMA*(cons[i][j+3][k][I_ENE]*unp3-cons[i][j-3][k][I_ENE]*unm3
                      +(pres[i][j+3][k]*unp3-pres[i][j-3][k]*unm3))
                   +DELTA*(cons[i][j+4][k][I_ENE]*unp4-cons[i][j-4][k][I_ENE]*unm4
                      +(pres[i][j+4][k]*unp4-pres[i][j-4][k]*unm4)))/dx[1];

                for(int nsp=0;nsp<nspec;nsp++){
                   flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
                      (ALPHA*(cons[i][j+1][k][I_SP+nsp]*unp1-cons[i][j-1][k][I_SP+nsp]*unm1)
                      +BETA *(cons[i][j+2][k][I_SP+nsp]*unp2-cons[i][j-2][k][I_SP+nsp]*unm2)
                      +GAMMA*(cons[i][j+3][k][I_SP+nsp]*unp3-cons[i][j-3][k][I_SP+nsp]*unm3)
                      +DELTA*(cons[i][j+4][k][I_SP+nsp]*unp4-cons[i][j-4][k][I_SP+nsp]*unm4))/dx[1];
                }

                unp1 = cons[i][j][k+1][I_MZ]/cons[i][j][k+1][I_RHO];
                unp2 = cons[i][j][k+2][I_MZ]/cons[i][j][k+2][I_RHO];
                unp3 = cons[i][j][k+3][I_MZ]/cons[i][j][k+3][I_RHO];
                unp4 = cons[i][j][k+4][I_MZ]/cons[i][j][k+4][I_RHO];

                unm1 = cons[i][j][k-1][I_MZ]/cons[i][j][k-1][I_RHO];
                unm2 = cons[i][j][k-2][I_MZ]/cons[i][j][k-2][I_RHO];
                unm3 = cons[i][j][k-3][I_MZ]/cons[i][j][k-3][I_RHO];
                unm4 = cons[i][j][k-4][I_MZ]/cons[i][j][k-4][I_RHO];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
                   (ALPHA*(cons[i][j][k+1][I_MZ]-cons[i][j][k-1][I_MZ])
                   +BETA *(cons[i][j][k+2][I_MZ]-cons[i][j][k-2][I_MZ])
                   +GAMMA*(cons[i][j][k+3][I_MZ]-cons[i][j][k-3][I_MZ])
                   +DELTA*(cons[i][j][k+4][I_MZ]-cons[i][j][k-4][I_MZ]))/dx[2];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
                   (ALPHA*(cons[i][j][k+1][I_MX]*unp1-cons[i][j][k-1][I_MX]*unm1)
                   +BETA *(cons[i][j][k+2][I_MX]*unp2-cons[i][j][k-2][I_MX]*unm2)
                   +GAMMA*(cons[i][j][k+3][I_MX]*unp3-cons[i][j][k-3][I_MX]*unm3)
                   +DELTA*(cons[i][j][k+4][I_MX]*unp4-cons[i][j][k-4][I_MX]*unm4))/dx[2];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
                   (ALPHA*(cons[i][j][k+1][I_MY]*unp1-cons[i][j][k-1][I_MY]*unm1)
                   +BETA *(cons[i][j][k+2][I_MY]*unp2-cons[i][j][k-2][I_MY]*unm2)
                   +GAMMA*(cons[i][j][k+3][I_MY]*unp3-cons[i][j][k-3][I_MY]*unm3)
                   +DELTA*(cons[i][j][k+4][I_MY]*unp4-cons[i][j][k-4][I_MY]*unm4))/dx[2];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
                   (ALPHA*(cons[i][j][k+1][I_MZ]*unp1-cons[i][j][k-1][I_MZ]*unm1
                      +(pres[i][j][k+1]-pres[i][j][k-1]))
                   +BETA *(cons[i][j][k+2][I_MZ]*unp2-cons[i][j][k-2][I_MZ]*unm2
                      +(pres[i][j][k+2]-pres[i][j][k-2]))
                   +GAMMA*(cons[i][j][k+3][I_MZ]*unp3-cons[i][j][k-3][I_MZ]*unm3
                      +(pres[i][j][k+3]-pres[i][j][k-3]))
                   +DELTA*(cons[i][j][k+4][I_MZ]*unp4-cons[i][j][k-4][I_MZ]*unm4
                      +(pres[i][j][k+4]-pres[i][j][k-4])))/dx[2];

                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE] -
                   (ALPHA*(cons[i][j][k+1][I_ENE]*unp1-cons[i][j][k-1][I_ENE]*unm1
                      +(pres[i][j][k+1]*unp1-pres[i][j][k-1]*unm1))
                   +BETA *(cons[i][j][k+2][I_ENE]*unp2-cons[i][j][k-2][I_ENE]*unm2
                      +(pres[i][j][k+2]*unp2-pres[i][j][k-2]*unm2))
                   +GAMMA*(cons[i][j][k+3][I_ENE]*unp3-cons[i][j][k-3][I_ENE]*unm3
                      +(pres[i][j][k+3]*unp3-pres[i][j][k-3]*unm3))
                   +DELTA*(cons[i][j][k+4][I_ENE]*unp4-cons[i][j][k-4][I_ENE]*unm4
                      +(pres[i][j][k+4]*unp4-pres[i][j][k-4]*unm4)))/dx[2];

                for(int nsp=0;nsp<nspec;nsp++){
                   flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
                      (ALPHA*(cons[i][j][k+1][I_SP+nsp]*unp1-cons[i][j][k-1][I_SP+nsp]*unm1)
                      +BETA *(cons[i][j][k+2][I_SP+nsp]*unp2-cons[i][j][k-2][I_SP+nsp]*unm2)
                      +GAMMA*(cons[i][j][k+3][I_SP+nsp]*unp3-cons[i][j][k-3][I_SP+nsp]*unm3)
                      +DELTA*(cons[i][j][k+4][I_SP+nsp]*unp4-cons[i][j][k-4][I_SP+nsp]*unm4))/dx[2];
                }
            }
        }
    }
}

void *hypterm_threaded(void *threadarg){
    struct thread_data *calc_params = (struct thread_data *) threadarg;
    int thread_id = calc_params->thread_id;
    int ns[3]={calc_params->n0s,calc_params->n1s,calc_params->n2s};
    int ne[3]={calc_params->n0e,calc_params->n1e,calc_params->n2e};
    int ng[3]={calc_params->ng0,calc_params->ng1,calc_params->ng2};
    double dx[3]={calc_params->dx0,calc_params->dx1,calc_params->dx2};
    int blocksize=calc_params->blocksize;

    int nspec=calc_params->nspec;
    double ***pres=calc_params->pres;
    double ****cons=calc_params->cons;
    double ****flux=calc_params->flux;
    for(int ii=ns[0]+ng[0];ii<ne[0]+ng[0];ii+=blocksize){
        for(int jj=ns[1]+ng[1];jj<ne[1]+ng[1];jj+=blocksize){
            for(int kk=ns[2]+ng[2];kk<ne[2]+ng[2];kk+=blocksize){
                for(int i=ii;i<std::min(ii+blocksize,ne[0]+ng[0]);i++){
                    for(int j=jj;j<std::min(jj+blocksize,ne[1]+ng[1]);j++){
                        for(int k=kk;k<std::min(kk+blocksize,ne[2]+ng[2]);k++){
                            double unp1=cons[i+1][j][k][I_MX]/cons[i+1][j][k][I_RHO];
                            double unp2=cons[i+2][j][k][I_MX]/cons[i+2][j][k][I_RHO];
                            double unp3=cons[i+3][j][k][I_MX]/cons[i+3][j][k][I_RHO];
                            double unp4=cons[i+4][j][k][I_MX]/cons[i+4][j][k][I_RHO];

                            double unm1=cons[i-1][j][k][I_MX]/cons[i-1][j][k][I_RHO];
                            double unm2=cons[i-2][j][k][I_MX]/cons[i-2][j][k][I_RHO];
                            double unm3=cons[i-3][j][k][I_MX]/cons[i-3][j][k][I_RHO];
                            double unm4=cons[i-4][j][k][I_MX]/cons[i-4][j][k][I_RHO];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
                            (ALPHA*(cons[i+1][j][k][I_MX]-cons[i-1][j][k][I_MX])
                            +BETA *(cons[i+2][j][k][I_MX]-cons[i-2][j][k][I_MX])
                            +GAMMA*(cons[i+3][j][k][I_MX]-cons[i-3][j][k][I_MX])
                            +DELTA*(cons[i+4][j][k][I_MX]-cons[i-4][j][k][I_MX]))/dx[0];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
                            (ALPHA*(cons[i+1][j][k][I_MX]*unp1-cons[i-1][j][k][I_MX]*unm1
                                  +(pres[i+1][j][k]-pres[i-1][j][k]))
                            +BETA *(cons[i+2][j][k][I_MX]*unp2-cons[i-2][j][k][I_MX]*unm2
                                  +(pres[i+2][j][k]-pres[i-2][j][k]))
                            +GAMMA*(cons[i+3][j][k][I_MX]*unp3-cons[i-3][j][k][I_MX]*unm3
                                  +(pres[i+3][j][k]-pres[i-3][j][k]))
                            +DELTA*(cons[i+4][j][k][I_MX]*unp4-cons[i-4][j][k][I_MX]*unm4
                                  +(pres[i+4][j][k]-pres[i-4][j][k])))/dx[0];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
                            (ALPHA*(cons[i+1][j][k][I_MY]*unp1-cons[i-1][j][k][I_MY]*unm1)
                            +BETA *(cons[i+2][j][k][I_MY]*unp2-cons[i-2][j][k][I_MY]*unm2)
                            +GAMMA*(cons[i+3][j][k][I_MY]*unp3-cons[i-3][j][k][I_MY]*unm3)
                            +DELTA*(cons[i+4][j][k][I_MY]*unp4-cons[i-4][j][k][I_MY]*unm4))/dx[0];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
                            (ALPHA*(cons[i+1][j][k][I_MZ]*unp1-cons[i-1][j][k][I_MZ]*unm1)
                            +BETA *(cons[i+2][j][k][I_MZ]*unp2-cons[i-2][j][k][I_MZ]*unm2)
                            +GAMMA*(cons[i+3][j][k][I_MZ]*unp3-cons[i-3][j][k][I_MZ]*unm3)
                            +DELTA*(cons[i+4][j][k][I_MZ]*unp4-cons[i-4][j][k][I_MZ]*unm4))/dx[0];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
                            (ALPHA*(cons[i+1][j][k][I_ENE]*unp1-cons[i-1][j][k][I_ENE]*unm1
                               +(pres[i+1][j][k]*unp1-pres[i-1][j][k]*unm1))
                            +BETA *(cons[i+2][j][k][I_ENE]*unp2-cons[i-2][j][k][I_ENE]*unm2
                               +(pres[i+2][j][k]*unp2-pres[i-2][j][k]*unm2))
                            +GAMMA*(cons[i+3][j][k][I_ENE]*unp3-cons[i-3][j][k][I_ENE]*unm3
                               +(pres[i+3][j][k]*unp3-pres[i-3][j][k]*unm3))
                            +DELTA*(cons[i+4][j][k][I_ENE]*unp4-cons[i-4][j][k][I_ENE]*unm4
                               +(pres[i+4][j][k]*unp4-pres[i-4][j][k]*unm4)))/dx[0];
                            
                            for(int nsp=0;nsp<nspec;nsp++){
                                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
                                (ALPHA*(cons[i+1][j][k][I_SP+nsp]*unp1-cons[i-1][j][k][I_SP+nsp]*unm1)
                                +BETA *(cons[i+2][j][k][I_SP+nsp]*unp2-cons[i-2][j][k][I_SP+nsp]*unm2)
                                +GAMMA*(cons[i+3][j][k][I_SP+nsp]*unp3-cons[i-3][j][k][I_SP+nsp]*unm3)
                                +DELTA*(cons[i+4][j][k][I_SP+nsp]*unp4-cons[i-4][j][k][I_SP+nsp]*unm4))/dx[0];
                            }

                            unp1 = cons[i][j+1][k][I_MY]/cons[i][j+1][k][I_RHO];
                            unp2 = cons[i][j+2][k][I_MY]/cons[i][j+2][k][I_RHO];
                            unp3 = cons[i][j+3][k][I_MY]/cons[i][j+3][k][I_RHO];
                            unp4 = cons[i][j+4][k][I_MY]/cons[i][j+4][k][I_RHO];
                            
                            unm1 = cons[i][j-1][k][I_MY]/cons[i][j-1][k][I_RHO];
                            unm2 = cons[i][j-2][k][I_MY]/cons[i][j-2][k][I_RHO];
                            unm3 = cons[i][j-3][k][I_MY]/cons[i][j-3][k][I_RHO];
                            unm4 = cons[i][j-4][k][I_MY]/cons[i][j-4][k][I_RHO];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
                               (ALPHA*(cons[i][j+1][k][I_MY]-cons[i][j-1][k][I_MY])
                               +BETA *(cons[i][j+2][k][I_MY]-cons[i][j-2][k][I_MY])
                               +GAMMA*(cons[i][j+3][k][I_MY]-cons[i][j-3][k][I_MY])
                               +DELTA*(cons[i][j+4][k][I_MY]-cons[i][j-4][k][I_MY]))/dx[1];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
                               (ALPHA*(cons[i][j+1][k][I_MX]*unp1-cons[i][j-1][k][I_MX]*unm1)
                               +BETA *(cons[i][j+2][k][I_MX]*unp2-cons[i][j-2][k][I_MX]*unm2)
                               +GAMMA*(cons[i][j+3][k][I_MX]*unp3-cons[i][j-3][k][I_MX]*unm3)
                               +DELTA*(cons[i][j+4][k][I_MX]*unp4-cons[i][j-4][k][I_MX]*unm4))/dx[1];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
                               (ALPHA*(cons[i][j+1][k][I_MY]*unp1-cons[i][j-1][k][I_MY]*unm1
                                  +(pres[i][j+1][k]-pres[i][j-1][k]))
                               +BETA *(cons[i][j+2][k][I_MY]*unp2-cons[i][j-2][k][I_MY]*unm2
                                  +(pres[i][j+2][k]-pres[i][j-2][k]))
                               +GAMMA*(cons[i][j+3][k][I_MY]*unp3-cons[i][j-3][k][I_MY]*unm3
                                  +(pres[i][j+3][k]-pres[i][j-3][k]))
                               +DELTA*(cons[i][j+4][k][I_MY]*unp4-cons[i][j-4][k][I_MY]*unm4
                                  +(pres[i][j+4][k]-pres[i][j-4][k])))/dx[1];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
                               (ALPHA*(cons[i][j+1][k][I_MZ]*unp1-cons[i][j-1][k][I_MZ]*unm1)
                               +BETA *(cons[i][j+2][k][I_MZ]*unp2-cons[i][j-2][k][I_MZ]*unm2)
                               +GAMMA*(cons[i][j+3][k][I_MZ]*unp3-cons[i][j-3][k][I_MZ]*unm3)
                               +DELTA*(cons[i][j+4][k][I_MZ]*unp4-cons[i][j-4][k][I_MZ]*unm4))/dx[1];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
                               (ALPHA*(cons[i][j+1][k][I_ENE]*unp1-cons[i][j-1][k][I_ENE]*unm1
                                  +(pres[i][j+1][k]*unp1-pres[i][j-1][k]*unm1))
                               +BETA *(cons[i][j+2][k][I_ENE]*unp2-cons[i][j-2][k][I_ENE]*unm2
                                  +(pres[i][j+2][k]*unp2-pres[i][j-2][k]*unm2))
                               +GAMMA*(cons[i][j+3][k][I_ENE]*unp3-cons[i][j-3][k][I_ENE]*unm3
                                  +(pres[i][j+3][k]*unp3-pres[i][j-3][k]*unm3))
                               +DELTA*(cons[i][j+4][k][I_ENE]*unp4-cons[i][j-4][k][I_ENE]*unm4
                                  +(pres[i][j+4][k]*unp4-pres[i][j-4][k]*unm4)))/dx[1];

                            for(int nsp=0;nsp<nspec;nsp++){
                               flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
                                  (ALPHA*(cons[i][j+1][k][I_SP+nsp]*unp1-cons[i][j-1][k][I_SP+nsp]*unm1)
                                  +BETA *(cons[i][j+2][k][I_SP+nsp]*unp2-cons[i][j-2][k][I_SP+nsp]*unm2)
                                  +GAMMA*(cons[i][j+3][k][I_SP+nsp]*unp3-cons[i][j-3][k][I_SP+nsp]*unm3)
                                  +DELTA*(cons[i][j+4][k][I_SP+nsp]*unp4-cons[i][j-4][k][I_SP+nsp]*unm4))/dx[1];
                            }
                            
                            unp1 = cons[i][j][k+1][I_MZ]/cons[i][j][k+1][I_RHO];
                            unp2 = cons[i][j][k+2][I_MZ]/cons[i][j][k+2][I_RHO];
                            unp3 = cons[i][j][k+3][I_MZ]/cons[i][j][k+3][I_RHO];
                            unp4 = cons[i][j][k+4][I_MZ]/cons[i][j][k+4][I_RHO];

                            unm1 = cons[i][j][k-1][I_MZ]/cons[i][j][k-1][I_RHO];
                            unm2 = cons[i][j][k-2][I_MZ]/cons[i][j][k-2][I_RHO];
                            unm3 = cons[i][j][k-3][I_MZ]/cons[i][j][k-3][I_RHO];
                            unm4 = cons[i][j][k-4][I_MZ]/cons[i][j][k-4][I_RHO];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
                               (ALPHA*(cons[i][j][k+1][I_MZ]-cons[i][j][k-1][I_MZ])
                               +BETA *(cons[i][j][k+2][I_MZ]-cons[i][j][k-2][I_MZ])
                               +GAMMA*(cons[i][j][k+3][I_MZ]-cons[i][j][k-3][I_MZ])
                               +DELTA*(cons[i][j][k+4][I_MZ]-cons[i][j][k-4][I_MZ]))/dx[2];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
                               (ALPHA*(cons[i][j][k+1][I_MX]*unp1-cons[i][j][k-1][I_MX]*unm1)
                               +BETA *(cons[i][j][k+2][I_MX]*unp2-cons[i][j][k-2][I_MX]*unm2)
                               +GAMMA*(cons[i][j][k+3][I_MX]*unp3-cons[i][j][k-3][I_MX]*unm3)
                               +DELTA*(cons[i][j][k+4][I_MX]*unp4-cons[i][j][k-4][I_MX]*unm4))/dx[2];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
                               (ALPHA*(cons[i][j][k+1][I_MY]*unp1-cons[i][j][k-1][I_MY]*unm1)
                               +BETA *(cons[i][j][k+2][I_MY]*unp2-cons[i][j][k-2][I_MY]*unm2)
                               +GAMMA*(cons[i][j][k+3][I_MY]*unp3-cons[i][j][k-3][I_MY]*unm3)
                               +DELTA*(cons[i][j][k+4][I_MY]*unp4-cons[i][j][k-4][I_MY]*unm4))/dx[2];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
                               (ALPHA*(cons[i][j][k+1][I_MZ]*unp1-cons[i][j][k-1][I_MZ]*unm1
                                  +(pres[i][j][k+1]-pres[i][j][k-1]))
                               +BETA *(cons[i][j][k+2][I_MZ]*unp2-cons[i][j][k-2][I_MZ]*unm2
                                  +(pres[i][j][k+2]-pres[i][j][k-2]))
                               +GAMMA*(cons[i][j][k+3][I_MZ]*unp3-cons[i][j][k-3][I_MZ]*unm3
                                  +(pres[i][j][k+3]-pres[i][j][k-3]))
                               +DELTA*(cons[i][j][k+4][I_MZ]*unp4-cons[i][j][k-4][I_MZ]*unm4
                                  +(pres[i][j][k+4]-pres[i][j][k-4])))/dx[2];

                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE] -
                               (ALPHA*(cons[i][j][k+1][I_ENE]*unp1-cons[i][j][k-1][I_ENE]*unm1
                                  +(pres[i][j][k+1]*unp1-pres[i][j][k-1]*unm1))
                               +BETA *(cons[i][j][k+2][I_ENE]*unp2-cons[i][j][k-2][I_ENE]*unm2
                                  +(pres[i][j][k+2]*unp2-pres[i][j][k-2]*unm2))
                               +GAMMA*(cons[i][j][k+3][I_ENE]*unp3-cons[i][j][k-3][I_ENE]*unm3
                                  +(pres[i][j][k+3]*unp3-pres[i][j][k-3]*unm3))
                               +DELTA*(cons[i][j][k+4][I_ENE]*unp4-cons[i][j][k-4][I_ENE]*unm4
                                  +(pres[i][j][k+4]*unp4-pres[i][j][k-4]*unm4)))/dx[2];

                            for(int nsp=0;nsp<nspec;nsp++){
                               flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
                                  (ALPHA*(cons[i][j][k+1][I_SP+nsp]*unp1-cons[i][j][k-1][I_SP+nsp]*unm1)
                                  +BETA *(cons[i][j][k+2][I_SP+nsp]*unp2-cons[i][j][k-2][I_SP+nsp]*unm2)
                                  +GAMMA*(cons[i][j][k+3][I_SP+nsp]*unp3-cons[i][j][k-3][I_SP+nsp]*unm3)
                                  +DELTA*(cons[i][j][k+4][I_SP+nsp]*unp4-cons[i][j][k-4][I_SP+nsp]*unm4))/dx[2];
                            }
                        }
                    }
                }
            }
        }
    }
}
