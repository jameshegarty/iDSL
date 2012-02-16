#include <pthread.h>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "constants.h"
#include "hypterm.h"

void init(const int *n,const int *ng,const double *dx,const int nspec,double ****cons,double ***pres){
    double scale[3]={0.02,0.02,0.02};

    for(int i0=0;i0<n[0]+2*ng[0];i0++){
        for(int i1=0;i1<n[1]+2*ng[1];i1++){
            for(int i2=0;i2<n[2]+2*ng[2];i2++){
                for(int i3=I_SP;i3<nspec+5;i3++){
                    cons[i0][i1][i2][i3]=0.0;
                }
            }
        }
    }

    for(int i=0;i<n[0]+2*ng[0];i++){
        double xloc=(i-ng[0]+1)*dx[0]/scale[0];
        for(int j=0;j<n[1]+2*ng[1];j++){
            double yloc=(j-ng[1]+1)*dx[1]/scale[1];
            for(int k=0;k<n[2]+2*ng[2];k++){
                double zloc=(k-ng[2]+1)*dx[2]/scale[2];
                double uvel=sin(xloc)*sin(2.0*yloc)*sin(3.0*zloc);
                double vvel=sin(2.0*xloc)*sin(4.0*yloc)*sin(zloc);
                double wvel=sin(3.0*xloc)*cos(2.0*yloc)*sin(2.0*zloc);
                double rholoc=1e-3+1e-5*sin(xloc)*cos(2.0*yloc)*cos(3.0*zloc);
                double ploc=1e6+1e-3*sin(2.0*xloc)*cos(2.0*yloc)*sin(2.0*zloc);

                cons[i][j][k][I_RHO]=rholoc;
                cons[i][j][k][I_MX]=rholoc*uvel;
                cons[i][j][k][I_MY]=rholoc*vvel;
                cons[i][j][k][I_MZ]=rholoc*wvel;
                cons[i][j][k][I_ENE]=ploc/0.4+rholoc*(uvel*uvel+vvel*vvel+wvel*wvel)/2.0;

                pres[i][j][k]=ploc;

                cons[i][j][k][I_SP]=0.2+0.1*uvel;
                cons[i][j][k][I_SP+1]=0.2+0.05*vvel;
                cons[i][j][k][I_SP+2]=0.2+0.03*wvel;
                cons[i][j][k][I_SP+3]=1.0-cons[i][j][k][I_SP]-cons[i][j][k][I_SP+1]-cons[i][j][k][I_SP+2];
            }
        }
    }
}

//void hypterm_serial(const int *n,const int *ng,const double *dx,const int nspec,double ****cons,double ***pres,double ****flux){
//    for(int i=ng[0];i<n[0]+ng[0];i++){
//        for(int j=ng[1];j<n[1]+ng[1];j++){
//            for(int k=ng[2];k<n[2]+ng[2];k++){
//                double unp1=cons[i+1][j][k][I_MX]/cons[i+1][j][k][I_RHO];
//                double unp2=cons[i+2][j][k][I_MX]/cons[i+2][j][k][I_RHO];
//                double unp3=cons[i+3][j][k][I_MX]/cons[i+3][j][k][I_RHO];
//                double unp4=cons[i+4][j][k][I_MX]/cons[i+4][j][k][I_RHO];
//
//                double unm1=cons[i-1][j][k][I_MX]/cons[i-1][j][k][I_RHO];
//                double unm2=cons[i-2][j][k][I_MX]/cons[i-2][j][k][I_RHO];
//                double unm3=cons[i-3][j][k][I_MX]/cons[i-3][j][k][I_RHO];
//                double unm4=cons[i-4][j][k][I_MX]/cons[i-4][j][k][I_RHO];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
//                (ALPHA*(cons[i+1][j][k][I_MX]-cons[i-1][j][k][I_MX])
//                +BETA *(cons[i+2][j][k][I_MX]-cons[i-2][j][k][I_MX])
//                +GAMMA*(cons[i+3][j][k][I_MX]-cons[i-3][j][k][I_MX])
//                +DELTA*(cons[i+4][j][k][I_MX]-cons[i-4][j][k][I_MX]))/dx[0];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
//                (ALPHA*(cons[i+1][j][k][I_MX]*unp1-cons[i-1][j][k][I_MX]*unm1
//                      +(pres[i+1][j][k]-pres[i-1][j][k]))
//                +BETA *(cons[i+2][j][k][I_MX]*unp2-cons[i-2][j][k][I_MX]*unm2
//                      +(pres[i+2][j][k]-pres[i-2][j][k]))
//                +GAMMA*(cons[i+3][j][k][I_MX]*unp3-cons[i-3][j][k][I_MX]*unm3
//                      +(pres[i+3][j][k]-pres[i-3][j][k]))
//                +DELTA*(cons[i+4][j][k][I_MX]*unp4-cons[i-4][j][k][I_MX]*unm4
//                      +(pres[i+4][j][k]-pres[i-4][j][k])))/dx[0];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
//                (ALPHA*(cons[i+1][j][k][I_MY]*unp1-cons[i-1][j][k][I_MY]*unm1)
//                +BETA *(cons[i+2][j][k][I_MY]*unp2-cons[i-2][j][k][I_MY]*unm2)
//                +GAMMA*(cons[i+3][j][k][I_MY]*unp3-cons[i-3][j][k][I_MY]*unm3)
//                +DELTA*(cons[i+4][j][k][I_MY]*unp4-cons[i-4][j][k][I_MY]*unm4))/dx[0];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
//                (ALPHA*(cons[i+1][j][k][I_MZ]*unp1-cons[i-1][j][k][I_MZ]*unm1)
//                +BETA *(cons[i+2][j][k][I_MZ]*unp2-cons[i-2][j][k][I_MZ]*unm2)
//                +GAMMA*(cons[i+3][j][k][I_MZ]*unp3-cons[i-3][j][k][I_MZ]*unm3)
//                +DELTA*(cons[i+4][j][k][I_MZ]*unp4-cons[i-4][j][k][I_MZ]*unm4))/dx[0];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
//                (ALPHA*(cons[i+1][j][k][I_ENE]*unp1-cons[i-1][j][k][I_ENE]*unm1
//                   +(pres[i+1][j][k]*unp1-pres[i-1][j][k]*unm1))
//                +BETA *(cons[i+2][j][k][I_ENE]*unp2-cons[i-2][j][k][I_ENE]*unm2
//                   +(pres[i+2][j][k]*unp2-pres[i-2][j][k]*unm2))
//                +GAMMA*(cons[i+3][j][k][I_ENE]*unp3-cons[i-3][j][k][I_ENE]*unm3
//                   +(pres[i+3][j][k]*unp3-pres[i-3][j][k]*unm3))
//                +DELTA*(cons[i+4][j][k][I_ENE]*unp4-cons[i-4][j][k][I_ENE]*unm4
//                   +(pres[i+4][j][k]*unp4-pres[i-4][j][k]*unm4)))/dx[0];
//                
//                for(int nsp=0;nsp<nspec;nsp++){
//                    flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
//                    (ALPHA*(cons[i+1][j][k][I_SP+nsp]*unp1-cons[i-1][j][k][I_SP+nsp]*unm1)
//                    +BETA *(cons[i+2][j][k][I_SP+nsp]*unp2-cons[i-2][j][k][I_SP+nsp]*unm2)
//                    +GAMMA*(cons[i+3][j][k][I_SP+nsp]*unp3-cons[i-3][j][k][I_SP+nsp]*unm3)
//                    +DELTA*(cons[i+4][j][k][I_SP+nsp]*unp4-cons[i-4][j][k][I_SP+nsp]*unm4))/dx[0];
//                }
//
//                unp1 = cons[i][j+1][k][I_MY]/cons[i][j+1][k][I_RHO];
//                unp2 = cons[i][j+2][k][I_MY]/cons[i][j+2][k][I_RHO];
//                unp3 = cons[i][j+3][k][I_MY]/cons[i][j+3][k][I_RHO];
//                unp4 = cons[i][j+4][k][I_MY]/cons[i][j+4][k][I_RHO];
//                
//                unm1 = cons[i][j-1][k][I_MY]/cons[i][j-1][k][I_RHO];
//                unm2 = cons[i][j-2][k][I_MY]/cons[i][j-2][k][I_RHO];
//                unm3 = cons[i][j-3][k][I_MY]/cons[i][j-3][k][I_RHO];
//                unm4 = cons[i][j-4][k][I_MY]/cons[i][j-4][k][I_RHO];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
//                   (ALPHA*(cons[i][j+1][k][I_MY]-cons[i][j-1][k][I_MY])
//                   +BETA *(cons[i][j+2][k][I_MY]-cons[i][j-2][k][I_MY])
//                   +GAMMA*(cons[i][j+3][k][I_MY]-cons[i][j-3][k][I_MY])
//                   +DELTA*(cons[i][j+4][k][I_MY]-cons[i][j-4][k][I_MY]))/dx[1];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
//                   (ALPHA*(cons[i][j+1][k][I_MX]*unp1-cons[i][j-1][k][I_MX]*unm1)
//                   +BETA *(cons[i][j+2][k][I_MX]*unp2-cons[i][j-2][k][I_MX]*unm2)
//                   +GAMMA*(cons[i][j+3][k][I_MX]*unp3-cons[i][j-3][k][I_MX]*unm3)
//                   +DELTA*(cons[i][j+4][k][I_MX]*unp4-cons[i][j-4][k][I_MX]*unm4))/dx[1];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
//                   (ALPHA*(cons[i][j+1][k][I_MY]*unp1-cons[i][j-1][k][I_MY]*unm1
//                      +(pres[i][j+1][k]-pres[i][j-1][k]))
//                   +BETA *(cons[i][j+2][k][I_MY]*unp2-cons[i][j-2][k][I_MY]*unm2
//                      +(pres[i][j+2][k]-pres[i][j-2][k]))
//                   +GAMMA*(cons[i][j+3][k][I_MY]*unp3-cons[i][j-3][k][I_MY]*unm3
//                      +(pres[i][j+3][k]-pres[i][j-3][k]))
//                   +DELTA*(cons[i][j+4][k][I_MY]*unp4-cons[i][j-4][k][I_MY]*unm4
//                      +(pres[i][j+4][k]-pres[i][j-4][k])))/dx[1];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
//                   (ALPHA*(cons[i][j+1][k][I_MZ]*unp1-cons[i][j-1][k][I_MZ]*unm1)
//                   +BETA *(cons[i][j+2][k][I_MZ]*unp2-cons[i][j-2][k][I_MZ]*unm2)
//                   +GAMMA*(cons[i][j+3][k][I_MZ]*unp3-cons[i][j-3][k][I_MZ]*unm3)
//                   +DELTA*(cons[i][j+4][k][I_MZ]*unp4-cons[i][j-4][k][I_MZ]*unm4))/dx[1];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
//                   (ALPHA*(cons[i][j+1][k][I_ENE]*unp1-cons[i][j-1][k][I_ENE]*unm1
//                      +(pres[i][j+1][k]*unp1-pres[i][j-1][k]*unm1))
//                   +BETA *(cons[i][j+2][k][I_ENE]*unp2-cons[i][j-2][k][I_ENE]*unm2
//                      +(pres[i][j+2][k]*unp2-pres[i][j-2][k]*unm2))
//                   +GAMMA*(cons[i][j+3][k][I_ENE]*unp3-cons[i][j-3][k][I_ENE]*unm3
//                      +(pres[i][j+3][k]*unp3-pres[i][j-3][k]*unm3))
//                   +DELTA*(cons[i][j+4][k][I_ENE]*unp4-cons[i][j-4][k][I_ENE]*unm4
//                      +(pres[i][j+4][k]*unp4-pres[i][j-4][k]*unm4)))/dx[1];
//
//                for(int nsp=0;nsp<nspec;nsp++){
//                   flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
//                      (ALPHA*(cons[i][j+1][k][I_SP+nsp]*unp1-cons[i][j-1][k][I_SP+nsp]*unm1)
//                      +BETA *(cons[i][j+2][k][I_SP+nsp]*unp2-cons[i][j-2][k][I_SP+nsp]*unm2)
//                      +GAMMA*(cons[i][j+3][k][I_SP+nsp]*unp3-cons[i][j-3][k][I_SP+nsp]*unm3)
//                      +DELTA*(cons[i][j+4][k][I_SP+nsp]*unp4-cons[i][j-4][k][I_SP+nsp]*unm4))/dx[1];
//                }
//
//                unp1 = cons[i][j][k+1][I_MZ]/cons[i][j][k+1][I_RHO];
//                unp2 = cons[i][j][k+2][I_MZ]/cons[i][j][k+2][I_RHO];
//                unp3 = cons[i][j][k+3][I_MZ]/cons[i][j][k+3][I_RHO];
//                unp4 = cons[i][j][k+4][I_MZ]/cons[i][j][k+4][I_RHO];
//
//                unm1 = cons[i][j][k-1][I_MZ]/cons[i][j][k-1][I_RHO];
//                unm2 = cons[i][j][k-2][I_MZ]/cons[i][j][k-2][I_RHO];
//                unm3 = cons[i][j][k-3][I_MZ]/cons[i][j][k-3][I_RHO];
//                unm4 = cons[i][j][k-4][I_MZ]/cons[i][j][k-4][I_RHO];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
//                   (ALPHA*(cons[i][j][k+1][I_MZ]-cons[i][j][k-1][I_MZ])
//                   +BETA *(cons[i][j][k+2][I_MZ]-cons[i][j][k-2][I_MZ])
//                   +GAMMA*(cons[i][j][k+3][I_MZ]-cons[i][j][k-3][I_MZ])
//                   +DELTA*(cons[i][j][k+4][I_MZ]-cons[i][j][k-4][I_MZ]))/dx[2];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
//                   (ALPHA*(cons[i][j][k+1][I_MX]*unp1-cons[i][j][k-1][I_MX]*unm1)
//                   +BETA *(cons[i][j][k+2][I_MX]*unp2-cons[i][j][k-2][I_MX]*unm2)
//                   +GAMMA*(cons[i][j][k+3][I_MX]*unp3-cons[i][j][k-3][I_MX]*unm3)
//                   +DELTA*(cons[i][j][k+4][I_MX]*unp4-cons[i][j][k-4][I_MX]*unm4))/dx[2];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
//                   (ALPHA*(cons[i][j][k+1][I_MY]*unp1-cons[i][j][k-1][I_MY]*unm1)
//                   +BETA *(cons[i][j][k+2][I_MY]*unp2-cons[i][j][k-2][I_MY]*unm2)
//                   +GAMMA*(cons[i][j][k+3][I_MY]*unp3-cons[i][j][k-3][I_MY]*unm3)
//                   +DELTA*(cons[i][j][k+4][I_MY]*unp4-cons[i][j][k-4][I_MY]*unm4))/dx[2];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
//                   (ALPHA*(cons[i][j][k+1][I_MZ]*unp1-cons[i][j][k-1][I_MZ]*unm1
//                      +(pres[i][j][k+1]-pres[i][j][k-1]))
//                   +BETA *(cons[i][j][k+2][I_MZ]*unp2-cons[i][j][k-2][I_MZ]*unm2
//                      +(pres[i][j][k+2]-pres[i][j][k-2]))
//                   +GAMMA*(cons[i][j][k+3][I_MZ]*unp3-cons[i][j][k-3][I_MZ]*unm3
//                      +(pres[i][j][k+3]-pres[i][j][k-3]))
//                   +DELTA*(cons[i][j][k+4][I_MZ]*unp4-cons[i][j][k-4][I_MZ]*unm4
//                      +(pres[i][j][k+4]-pres[i][j][k-4])))/dx[2];
//
//                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE] -
//                   (ALPHA*(cons[i][j][k+1][I_ENE]*unp1-cons[i][j][k-1][I_ENE]*unm1
//                      +(pres[i][j][k+1]*unp1-pres[i][j][k-1]*unm1))
//                   +BETA *(cons[i][j][k+2][I_ENE]*unp2-cons[i][j][k-2][I_ENE]*unm2
//                      +(pres[i][j][k+2]*unp2-pres[i][j][k-2]*unm2))
//                   +GAMMA*(cons[i][j][k+3][I_ENE]*unp3-cons[i][j][k-3][I_ENE]*unm3
//                      +(pres[i][j][k+3]*unp3-pres[i][j][k-3]*unm3))
//                   +DELTA*(cons[i][j][k+4][I_ENE]*unp4-cons[i][j][k-4][I_ENE]*unm4
//                      +(pres[i][j][k+4]*unp4-pres[i][j][k-4]*unm4)))/dx[2];
//
//                for(int nsp=0;nsp<nspec;nsp++){
//                   flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
//                      (ALPHA*(cons[i][j][k+1][I_SP+nsp]*unp1-cons[i][j][k-1][I_SP+nsp]*unm1)
//                      +BETA *(cons[i][j][k+2][I_SP+nsp]*unp2-cons[i][j][k-2][I_SP+nsp]*unm2)
//                      +GAMMA*(cons[i][j][k+3][I_SP+nsp]*unp3-cons[i][j][k-3][I_SP+nsp]*unm3)
//                      +DELTA*(cons[i][j][k+4][I_SP+nsp]*unp4-cons[i][j][k-4][I_SP+nsp]*unm4))/dx[2];
//                }
//            }
//        }
//    }
//}
//
//
//void *hypterm_threaded(void *threadarg){
//    struct thread_data *calc_params = (struct thread_data *) threadarg;
//    int thread_id = calc_params->thread_id;
//    int ns[3]={calc_params->n0s,calc_params->n1s,calc_params->n2s};
//    int ne[3]={calc_params->n0e,calc_params->n1e,calc_params->n2e};
//    int ng[3]={calc_params->ng0,calc_params->ng1,calc_params->ng2};
//    double dx[3]={calc_params->dx0,calc_params->dx1,calc_params->dx2};
//    int blocksize=calc_params->blocksize;
//
//    int nspec=calc_params->nspec;
//    double ***pres=calc_params->pres;
//    double ****cons=calc_params->cons;
//    double ****flux=calc_params->flux;
//    for(int ii=ns[0]+ng[0];ii<ne[0]+ng[0];ii+=blocksize){
//        for(int jj=ns[1]+ng[1];jj<ne[1]+ng[1];jj+=blocksize){
//            for(int kk=ns[2]+ng[2];kk<ne[2]+ng[2];kk+=blocksize){
//                for(int i=ii;i<std::min(ii+blocksize,ne[0]+ng[0]);i++){
//                    for(int j=jj;j<std::min(jj+blocksize,ne[1]+ng[1]);j++){
//                        for(int k=kk;k<std::min(kk+blocksize,ne[2]+ng[2]);k++){
//                            double unp1=cons[i+1][j][k][I_MX]/cons[i+1][j][k][I_RHO];
//                            double unp2=cons[i+2][j][k][I_MX]/cons[i+2][j][k][I_RHO];
//                            double unp3=cons[i+3][j][k][I_MX]/cons[i+3][j][k][I_RHO];
//                            double unp4=cons[i+4][j][k][I_MX]/cons[i+4][j][k][I_RHO];
//
//                            double unm1=cons[i-1][j][k][I_MX]/cons[i-1][j][k][I_RHO];
//                            double unm2=cons[i-2][j][k][I_MX]/cons[i-2][j][k][I_RHO];
//                            double unm3=cons[i-3][j][k][I_MX]/cons[i-3][j][k][I_RHO];
//                            double unm4=cons[i-4][j][k][I_MX]/cons[i-4][j][k][I_RHO];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
//                            (ALPHA*(cons[i+1][j][k][I_MX]-cons[i-1][j][k][I_MX])
//                            +BETA *(cons[i+2][j][k][I_MX]-cons[i-2][j][k][I_MX])
//                            +GAMMA*(cons[i+3][j][k][I_MX]-cons[i-3][j][k][I_MX])
//                            +DELTA*(cons[i+4][j][k][I_MX]-cons[i-4][j][k][I_MX]))/dx[0];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
//                            (ALPHA*(cons[i+1][j][k][I_MX]*unp1-cons[i-1][j][k][I_MX]*unm1
//                                  +(pres[i+1][j][k]-pres[i-1][j][k]))
//                            +BETA *(cons[i+2][j][k][I_MX]*unp2-cons[i-2][j][k][I_MX]*unm2
//                                  +(pres[i+2][j][k]-pres[i-2][j][k]))
//                            +GAMMA*(cons[i+3][j][k][I_MX]*unp3-cons[i-3][j][k][I_MX]*unm3
//                                  +(pres[i+3][j][k]-pres[i-3][j][k]))
//                            +DELTA*(cons[i+4][j][k][I_MX]*unp4-cons[i-4][j][k][I_MX]*unm4
//                                  +(pres[i+4][j][k]-pres[i-4][j][k])))/dx[0];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
//                            (ALPHA*(cons[i+1][j][k][I_MY]*unp1-cons[i-1][j][k][I_MY]*unm1)
//                            +BETA *(cons[i+2][j][k][I_MY]*unp2-cons[i-2][j][k][I_MY]*unm2)
//                            +GAMMA*(cons[i+3][j][k][I_MY]*unp3-cons[i-3][j][k][I_MY]*unm3)
//                            +DELTA*(cons[i+4][j][k][I_MY]*unp4-cons[i-4][j][k][I_MY]*unm4))/dx[0];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
//                            (ALPHA*(cons[i+1][j][k][I_MZ]*unp1-cons[i-1][j][k][I_MZ]*unm1)
//                            +BETA *(cons[i+2][j][k][I_MZ]*unp2-cons[i-2][j][k][I_MZ]*unm2)
//                            +GAMMA*(cons[i+3][j][k][I_MZ]*unp3-cons[i-3][j][k][I_MZ]*unm3)
//                            +DELTA*(cons[i+4][j][k][I_MZ]*unp4-cons[i-4][j][k][I_MZ]*unm4))/dx[0];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
//                            (ALPHA*(cons[i+1][j][k][I_ENE]*unp1-cons[i-1][j][k][I_ENE]*unm1
//                               +(pres[i+1][j][k]*unp1-pres[i-1][j][k]*unm1))
//                            +BETA *(cons[i+2][j][k][I_ENE]*unp2-cons[i-2][j][k][I_ENE]*unm2
//                               +(pres[i+2][j][k]*unp2-pres[i-2][j][k]*unm2))
//                            +GAMMA*(cons[i+3][j][k][I_ENE]*unp3-cons[i-3][j][k][I_ENE]*unm3
//                               +(pres[i+3][j][k]*unp3-pres[i-3][j][k]*unm3))
//                            +DELTA*(cons[i+4][j][k][I_ENE]*unp4-cons[i-4][j][k][I_ENE]*unm4
//                               +(pres[i+4][j][k]*unp4-pres[i-4][j][k]*unm4)))/dx[0];
//                            
//                            for(int nsp=0;nsp<nspec;nsp++){
//                                flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
//                                (ALPHA*(cons[i+1][j][k][I_SP+nsp]*unp1-cons[i-1][j][k][I_SP+nsp]*unm1)
//                                +BETA *(cons[i+2][j][k][I_SP+nsp]*unp2-cons[i-2][j][k][I_SP+nsp]*unm2)
//                                +GAMMA*(cons[i+3][j][k][I_SP+nsp]*unp3-cons[i-3][j][k][I_SP+nsp]*unm3)
//                                +DELTA*(cons[i+4][j][k][I_SP+nsp]*unp4-cons[i-4][j][k][I_SP+nsp]*unm4))/dx[0];
//                            }
//
//                            unp1 = cons[i][j+1][k][I_MY]/cons[i][j+1][k][I_RHO];
//                            unp2 = cons[i][j+2][k][I_MY]/cons[i][j+2][k][I_RHO];
//                            unp3 = cons[i][j+3][k][I_MY]/cons[i][j+3][k][I_RHO];
//                            unp4 = cons[i][j+4][k][I_MY]/cons[i][j+4][k][I_RHO];
//                            
//                            unm1 = cons[i][j-1][k][I_MY]/cons[i][j-1][k][I_RHO];
//                            unm2 = cons[i][j-2][k][I_MY]/cons[i][j-2][k][I_RHO];
//                            unm3 = cons[i][j-3][k][I_MY]/cons[i][j-3][k][I_RHO];
//                            unm4 = cons[i][j-4][k][I_MY]/cons[i][j-4][k][I_RHO];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
//                               (ALPHA*(cons[i][j+1][k][I_MY]-cons[i][j-1][k][I_MY])
//                               +BETA *(cons[i][j+2][k][I_MY]-cons[i][j-2][k][I_MY])
//                               +GAMMA*(cons[i][j+3][k][I_MY]-cons[i][j-3][k][I_MY])
//                               +DELTA*(cons[i][j+4][k][I_MY]-cons[i][j-4][k][I_MY]))/dx[1];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
//                               (ALPHA*(cons[i][j+1][k][I_MX]*unp1-cons[i][j-1][k][I_MX]*unm1)
//                               +BETA *(cons[i][j+2][k][I_MX]*unp2-cons[i][j-2][k][I_MX]*unm2)
//                               +GAMMA*(cons[i][j+3][k][I_MX]*unp3-cons[i][j-3][k][I_MX]*unm3)
//                               +DELTA*(cons[i][j+4][k][I_MX]*unp4-cons[i][j-4][k][I_MX]*unm4))/dx[1];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
//                               (ALPHA*(cons[i][j+1][k][I_MY]*unp1-cons[i][j-1][k][I_MY]*unm1
//                                  +(pres[i][j+1][k]-pres[i][j-1][k]))
//                               +BETA *(cons[i][j+2][k][I_MY]*unp2-cons[i][j-2][k][I_MY]*unm2
//                                  +(pres[i][j+2][k]-pres[i][j-2][k]))
//                               +GAMMA*(cons[i][j+3][k][I_MY]*unp3-cons[i][j-3][k][I_MY]*unm3
//                                  +(pres[i][j+3][k]-pres[i][j-3][k]))
//                               +DELTA*(cons[i][j+4][k][I_MY]*unp4-cons[i][j-4][k][I_MY]*unm4
//                                  +(pres[i][j+4][k]-pres[i][j-4][k])))/dx[1];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
//                               (ALPHA*(cons[i][j+1][k][I_MZ]*unp1-cons[i][j-1][k][I_MZ]*unm1)
//                               +BETA *(cons[i][j+2][k][I_MZ]*unp2-cons[i][j-2][k][I_MZ]*unm2)
//                               +GAMMA*(cons[i][j+3][k][I_MZ]*unp3-cons[i][j-3][k][I_MZ]*unm3)
//                               +DELTA*(cons[i][j+4][k][I_MZ]*unp4-cons[i][j-4][k][I_MZ]*unm4))/dx[1];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]-
//                               (ALPHA*(cons[i][j+1][k][I_ENE]*unp1-cons[i][j-1][k][I_ENE]*unm1
//                                  +(pres[i][j+1][k]*unp1-pres[i][j-1][k]*unm1))
//                               +BETA *(cons[i][j+2][k][I_ENE]*unp2-cons[i][j-2][k][I_ENE]*unm2
//                                  +(pres[i][j+2][k]*unp2-pres[i][j-2][k]*unm2))
//                               +GAMMA*(cons[i][j+3][k][I_ENE]*unp3-cons[i][j-3][k][I_ENE]*unm3
//                                  +(pres[i][j+3][k]*unp3-pres[i][j-3][k]*unm3))
//                               +DELTA*(cons[i][j+4][k][I_ENE]*unp4-cons[i][j-4][k][I_ENE]*unm4
//                                  +(pres[i][j+4][k]*unp4-pres[i][j-4][k]*unm4)))/dx[1];
//
//                            for(int nsp=0;nsp<nspec;nsp++){
//                               flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
//                                  (ALPHA*(cons[i][j+1][k][I_SP+nsp]*unp1-cons[i][j-1][k][I_SP+nsp]*unm1)
//                                  +BETA *(cons[i][j+2][k][I_SP+nsp]*unp2-cons[i][j-2][k][I_SP+nsp]*unm2)
//                                  +GAMMA*(cons[i][j+3][k][I_SP+nsp]*unp3-cons[i][j-3][k][I_SP+nsp]*unm3)
//                                  +DELTA*(cons[i][j+4][k][I_SP+nsp]*unp4-cons[i][j-4][k][I_SP+nsp]*unm4))/dx[1];
//                            }
//                            
//                            unp1 = cons[i][j][k+1][I_MZ]/cons[i][j][k+1][I_RHO];
//                            unp2 = cons[i][j][k+2][I_MZ]/cons[i][j][k+2][I_RHO];
//                            unp3 = cons[i][j][k+3][I_MZ]/cons[i][j][k+3][I_RHO];
//                            unp4 = cons[i][j][k+4][I_MZ]/cons[i][j][k+4][I_RHO];
//
//                            unm1 = cons[i][j][k-1][I_MZ]/cons[i][j][k-1][I_RHO];
//                            unm2 = cons[i][j][k-2][I_MZ]/cons[i][j][k-2][I_RHO];
//                            unm3 = cons[i][j][k-3][I_MZ]/cons[i][j][k-3][I_RHO];
//                            unm4 = cons[i][j][k-4][I_MZ]/cons[i][j][k-4][I_RHO];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO]-
//                               (ALPHA*(cons[i][j][k+1][I_MZ]-cons[i][j][k-1][I_MZ])
//                               +BETA *(cons[i][j][k+2][I_MZ]-cons[i][j][k-2][I_MZ])
//                               +GAMMA*(cons[i][j][k+3][I_MZ]-cons[i][j][k-3][I_MZ])
//                               +DELTA*(cons[i][j][k+4][I_MZ]-cons[i][j][k-4][I_MZ]))/dx[2];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MX]-
//                               (ALPHA*(cons[i][j][k+1][I_MX]*unp1-cons[i][j][k-1][I_MX]*unm1)
//                               +BETA *(cons[i][j][k+2][I_MX]*unp2-cons[i][j][k-2][I_MX]*unm2)
//                               +GAMMA*(cons[i][j][k+3][I_MX]*unp3-cons[i][j][k-3][I_MX]*unm3)
//                               +DELTA*(cons[i][j][k+4][I_MX]*unp4-cons[i][j][k-4][I_MX]*unm4))/dx[2];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MY]-
//                               (ALPHA*(cons[i][j][k+1][I_MY]*unp1-cons[i][j][k-1][I_MY]*unm1)
//                               +BETA *(cons[i][j][k+2][I_MY]*unp2-cons[i][j][k-2][I_MY]*unm2)
//                               +GAMMA*(cons[i][j][k+3][I_MY]*unp3-cons[i][j][k-3][I_MY]*unm3)
//                               +DELTA*(cons[i][j][k+4][I_MY]*unp4-cons[i][j][k-4][I_MY]*unm4))/dx[2];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_MZ]-
//                               (ALPHA*(cons[i][j][k+1][I_MZ]*unp1-cons[i][j][k-1][I_MZ]*unm1
//                                  +(pres[i][j][k+1]-pres[i][j][k-1]))
//                               +BETA *(cons[i][j][k+2][I_MZ]*unp2-cons[i][j][k-2][I_MZ]*unm2
//                                  +(pres[i][j][k+2]-pres[i][j][k-2]))
//                               +GAMMA*(cons[i][j][k+3][I_MZ]*unp3-cons[i][j][k-3][I_MZ]*unm3
//                                  +(pres[i][j][k+3]-pres[i][j][k-3]))
//                               +DELTA*(cons[i][j][k+4][I_MZ]*unp4-cons[i][j][k-4][I_MZ]*unm4
//                                  +(pres[i][j][k+4]-pres[i][j][k-4])))/dx[2];
//
//                            flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_ENE] -
//                               (ALPHA*(cons[i][j][k+1][I_ENE]*unp1-cons[i][j][k-1][I_ENE]*unm1
//                                  +(pres[i][j][k+1]*unp1-pres[i][j][k-1]*unm1))
//                               +BETA *(cons[i][j][k+2][I_ENE]*unp2-cons[i][j][k-2][I_ENE]*unm2
//                                  +(pres[i][j][k+2]*unp2-pres[i][j][k-2]*unm2))
//                               +GAMMA*(cons[i][j][k+3][I_ENE]*unp3-cons[i][j][k-3][I_ENE]*unm3
//                                  +(pres[i][j][k+3]*unp3-pres[i][j][k-3]*unm3))
//                               +DELTA*(cons[i][j][k+4][I_ENE]*unp4-cons[i][j][k-4][I_ENE]*unm4
//                                  +(pres[i][j][k+4]*unp4-pres[i][j][k-4]*unm4)))/dx[2];
//
//                            for(int nsp=0;nsp<nspec;nsp++){
//                               flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]=flux[i-ng[0]][j-ng[1]][k-ng[2]][I_SP+nsp]-
//                                  (ALPHA*(cons[i][j][k+1][I_SP+nsp]*unp1-cons[i][j][k-1][I_SP+nsp]*unm1)
//                                  +BETA *(cons[i][j][k+2][I_SP+nsp]*unp2-cons[i][j][k-2][I_SP+nsp]*unm2)
//                                  +GAMMA*(cons[i][j][k+3][I_SP+nsp]*unp3-cons[i][j][k-3][I_SP+nsp]*unm3)
//                                  +DELTA*(cons[i][j][k+4][I_SP+nsp]*unp4-cons[i][j][k-4][I_SP+nsp]*unm4))/dx[2];
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

int main(int argc,char **argv){
    if(argc < 3){
        std::cout << "Usage: <executable> <number_of_threads> <blocksize>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int numthreads=atoi(argv[1]);
    int blocksize=atoi(argv[2]);

    std::cout << "Block Size (Double Words)=" << blocksize << std::endl;
    std::cout << "Number of Threads=" << numthreads << std::endl;

    const int nspec=9;
    const double dx[3]={1e-3,1e-3,1e-3};
    
    const int n[3]={64,64,64};
    const int ng[3]={4,4,4};

    double ***pres=new double**[n[0]+2*ng[0]];
    for(int i=0;i<n[0]+2*ng[0];i++){
        pres[i]=new double*[n[1]+2*ng[1]];
        for(int j=0;j<n[1]+2*ng[1];j++){
            pres[i][j]=new double[n[2]+2*ng[2]];
        }
    }
    double ****cons=new double***[n[0]+2*ng[0]];
    for(int i=0;i<n[0]+2*ng[0];i++){
        cons[i]=new double**[n[1]+2*ng[1]];
        for(int j=0;j<n[1]+2*ng[1];j++){
            cons[i][j]=new double*[n[2]+2*ng[2]];
            for(int k=0;k<n[2]+2*ng[2];k++){
                cons[i][j][k]=new double[nspec+5];
            }
        }
    }
    double ****flux=new double***[n[0]];
    for(int i=0;i<n[0];i++){
        flux[i]=new double**[n[1]];
        for(int j=0;j<n[1];j++){
            flux[i][j]=new double*[n[2]];
            for(int k=0;k<n[2];k++){
                flux[i][j][k]=new double[nspec+5];
            }
        }
    }

    timespec start_threaded;
    timespec start_serial;
    timespec end_threaded;
    timespec end_serial;
    init(n,ng,dx,nspec,cons,pres);
    clock_gettime(CLOCK_REALTIME,&start_threaded);
    pthread_t threads[numthreads];
    struct thread_data thread_data_array[numthreads];
    void *status;
    for(int t=0;t<numthreads;t++){
        thread_data_array[t].thread_id=t;
        thread_data_array[t].n0s=(t%4)*(n[0]/4);
        thread_data_array[t].n1s=((t/4)%4)*(n[1]/4);
        thread_data_array[t].n2s=((t/16)%2)*(n[2]/2);
        thread_data_array[t].n0e=((t%4)+1)*(n[0]/4);
        thread_data_array[t].n1e=(((t/4)%4)+1)*(n[1]/4);
        thread_data_array[t].n2e=(((t/16)%2)+1)*(n[2]/2);
        //thread_data_array[t].n0s=0;
        //thread_data_array[t].n1s=0;
        //thread_data_array[t].n2s=0;
        //thread_data_array[t].n0e=n[0];
        //thread_data_array[t].n1e=n[1];
        //thread_data_array[t].n2e=n[2];
        thread_data_array[t].ng0=ng[0];
        thread_data_array[t].ng1=ng[1];
        thread_data_array[t].ng2=ng[2];
        thread_data_array[t].dx0=dx[0];
        thread_data_array[t].dx1=dx[1];
        thread_data_array[t].dx2=dx[2];
        thread_data_array[t].nspec=nspec;
        thread_data_array[t].pres=pres;
        thread_data_array[t].cons=cons;
        thread_data_array[t].flux=flux;
        thread_data_array[t].blocksize=blocksize;
        std::cout << "Creating thread " << t << std::endl;
        int rc=pthread_create(&threads[t],NULL,hypterm_threaded,(void*)&thread_data_array[t]);
        if(rc){
            std::cout << "ERROR; return code from pthread_create() is " << rc << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    for(int t=0;t<numthreads;t++){
        pthread_join(threads[t],&status);
    }
    clock_gettime(CLOCK_REALTIME,&end_threaded);
    timespec diff_threaded;
    diff_threaded.tv_sec=end_threaded.tv_sec-start_threaded.tv_sec;
    diff_threaded.tv_nsec=end_threaded.tv_nsec-start_threaded.tv_nsec;
    std::cout << "Threaded Code Runtime: " << ((double)diff_threaded.tv_sec + ((double)diff_threaded.tv_nsec/1E9)) << std::endl;
    

    double fluxmag[nspec+5];
    for(int i=0;i<nspec+5;i++){
        fluxmag[i]=0.0;
    }

    for(int ns=0;ns<nspec+5;ns++){
        for(int i=0;i<n[0];i++){
            for(int j=0;j<n[1];j++){
                for(int k=0;k<n[2];k++){
                    fluxmag[ns]=fluxmag[ns]+flux[i][j][k][ns]*flux[i][j][k][ns];
                }
            }
        }
    }
    
    for(int ns=0;ns<nspec+5;ns++){
        std::cout<<"threaded: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    for(int ns=0;ns<nspec+5;ns++){
        for(int i=0;i<n[0];i++){
            for(int j=0;j<n[1];j++){
                for(int k=0;k<n[2];k++){
                    flux[i][j][k][ns]=0.0;
                }
            }
        }
    }   
    
    for(int i=0;i<nspec+5;i++){
        fluxmag[i]=0.0;
    }

    clock_gettime(CLOCK_REALTIME,&start_serial);
    hypterm_serial(n,ng,dx,nspec,cons,pres,flux);
    clock_gettime(CLOCK_REALTIME,&end_serial);
    timespec diff_serial;
    diff_serial.tv_sec=end_serial.tv_sec-start_serial.tv_sec;
    diff_serial.tv_nsec=end_serial.tv_nsec-start_serial.tv_nsec;
    std::cout << "Serial Code Runtime: " << ((double)diff_serial.tv_sec + ((double)diff_serial.tv_nsec/1E9)) << std::endl;

    for(int ns=0;ns<nspec+5;ns++){
        for(int i=0;i<n[0];i++){
            for(int j=0;j<n[1];j++){
                for(int k=0;k<n[2];k++){
                    fluxmag[ns]=fluxmag[ns]+flux[i][j][k][ns]*flux[i][j][k][ns];
                }
            }
        }
    }
    
    for(int ns=0;ns<nspec+5;ns++){
        std::cout<<"serial: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    for(int i=0;i<n[0]+2*ng[0];i++){
        for(int j=0;j<n[1]+2*ng[1];j++){
            delete[] pres[i][j];
        }
        delete[] pres[i];
    }
    delete[] pres;

    for(int i=0;i<n[0]+2*ng[0];i++){
        for(int j=0;j<n[1]+2*ng[1];j++){
            for(int k=0;k<n[2]+2*ng[2];k++){
                delete[] cons[i][j][k];
            }
            delete[] cons[i][j];
        }
        delete[] cons[i];
    }
    delete[] cons;

    for(int i=0;i<n[0];i++){
        for(int j=0;j<n[1];j++){
            for(int k=0;k<n[2];k++){
                delete[] flux[i][j][k];
            }
            delete[] flux[i][j];
        }
        delete[] flux[i];
    }
    delete[] flux;
    
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
