#include <iostream>
#include "hypterm.h"
//#include "hypterm_ispc.h"

void hypterm_serial(int *n,int *ns,int *ne,int *ng,double *dx,int nspec,double *cons,double *pres,double *flux,int blocksize){
    int z4d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(nspec+5);
    int y4d_offset=(n[1]+2*ng[1])*(nspec+5);
    int x4d_offset=(nspec+5);
    int s4d_offset=1;
    int z3d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1]);
    int y3d_offset=(n[1]+2*ng[1]);
    int x3d_offset=1;

    for(int jj=ns[1]+ng[1];jj<ne[1]+ng[1];jj+=blocksize){
        for(int ii=ns[0]+ng[0];ii<ne[0]+ng[0];ii+=blocksize){
            for(int k=ns[2]+ng[2];k<ne[2]+ng[2];k++){
                for(int j=jj;j<std::min(jj+blocksize,ne[1]+ng[1]);j++){
                    for(int i=ii;i<std::min(ii+blocksize,ne[0]+ng[0]);i++){

                        int fijk=(k-ng[2])*n[1]*n[0]*(nspec+5)+(j-ng[1])*n[0]*(nspec+5)+(i-ng[0])*(nspec+5);
                        
                        int ip1jk=k*z4d_offset+j*y4d_offset+(i+1)*x4d_offset;
                        int ip2jk=k*z4d_offset+j*y4d_offset+(i+2)*x4d_offset;
                        int ip3jk=k*z4d_offset+j*y4d_offset+(i+3)*x4d_offset;
                        int ip4jk=k*z4d_offset+j*y4d_offset+(i+4)*x4d_offset;

                        int im1jk=k*z4d_offset+j*y4d_offset+(i-1)*x4d_offset;
                        int im2jk=k*z4d_offset+j*y4d_offset+(i-2)*x4d_offset;
                        int im3jk=k*z4d_offset+j*y4d_offset+(i-3)*x4d_offset;
                        int im4jk=k*z4d_offset+j*y4d_offset+(i-4)*x4d_offset;

                        double unp1=cons[ip1jk+I_MX]/cons[ip1jk+I_RHO];
                        double unp2=cons[ip2jk+I_MX]/cons[ip2jk+I_RHO];
                        double unp3=cons[ip3jk+I_MX]/cons[ip3jk+I_RHO];
                        double unp4=cons[ip4jk+I_MX]/cons[ip4jk+I_RHO];
                                                                     
                        double unm1=cons[im1jk+I_MX]/cons[im1jk+I_RHO];
                        double unm2=cons[im2jk+I_MX]/cons[im2jk+I_RHO];
                        double unm3=cons[im3jk+I_MX]/cons[im3jk+I_RHO];
                        double unm4=cons[im4jk+I_MX]/cons[im4jk+I_RHO];

                        flux[fijk+I_RHO]-=
                        (ALPHA*(cons[ip1jk+I_MX]-cons[im1jk+I_MX])
                        +BETA *(cons[ip2jk+I_MX]-cons[im2jk+I_MX])
                        +GAMMA*(cons[ip3jk+I_MX]-cons[im3jk+I_MX])
                        +DELTA*(cons[ip4jk+I_MX]-cons[im4jk+I_MX]))/dx[0];
                        
                        flux[fijk+I_MX]-=
                        (ALPHA*(cons[ip1jk+I_MX]*unp1-cons[im1jk+I_MX]*unm1+(pres[ip1jk/(nspec+5)]-pres[im1jk/(nspec+5)]))
                        +BETA *(cons[ip2jk+I_MX]*unp2-cons[im2jk+I_MX]*unm2+(pres[ip2jk/(nspec+5)]-pres[im2jk/(nspec+5)]))
                        +GAMMA*(cons[ip3jk+I_MX]*unp3-cons[im3jk+I_MX]*unm3+(pres[ip3jk/(nspec+5)]-pres[im3jk/(nspec+5)]))
                        +DELTA*(cons[ip4jk+I_MX]*unp4-cons[im4jk+I_MX]*unm4+(pres[ip4jk/(nspec+5)]-pres[im4jk/(nspec+5)])))/dx[0];

                        flux[fijk+I_MY]-=
                        (ALPHA*(cons[ip1jk+I_MY]*unp1-cons[im1jk+I_MY]*unm1)
                        +BETA *(cons[ip2jk+I_MY]*unp2-cons[im2jk+I_MY]*unm2)
                        +GAMMA*(cons[ip3jk+I_MY]*unp3-cons[im3jk+I_MY]*unm3)
                        +DELTA*(cons[ip4jk+I_MY]*unp4-cons[im4jk+I_MY]*unm4))/dx[0];

                        flux[fijk+I_MZ]-=
                        (ALPHA*(cons[ip1jk+I_MZ]*unp1-cons[im1jk+I_MZ]*unm1)
                        +BETA *(cons[ip2jk+I_MZ]*unp2-cons[im2jk+I_MZ]*unm2)
                        +GAMMA*(cons[ip3jk+I_MZ]*unp3-cons[im3jk+I_MZ]*unm3)
                        +DELTA*(cons[ip4jk+I_MZ]*unp4-cons[im4jk+I_MZ]*unm4))/dx[0];

                        flux[fijk+I_ENE]-=
                        (ALPHA*(cons[ip1jk+I_ENE]*unp1-cons[im1jk+I_ENE]*unm1+(pres[ip1jk/(nspec+5)]*unp1-pres[im1jk/(nspec+5)]*unm1))
                        +BETA *(cons[ip2jk+I_ENE]*unp2-cons[im2jk+I_ENE]*unm2+(pres[ip2jk/(nspec+5)]*unp2-pres[im2jk/(nspec+5)]*unm2))
                        +GAMMA*(cons[ip3jk+I_ENE]*unp3-cons[im3jk+I_ENE]*unm3+(pres[ip3jk/(nspec+5)]*unp3-pres[im3jk/(nspec+5)]*unm3))
                        +DELTA*(cons[ip4jk+I_ENE]*unp4-cons[im4jk+I_ENE]*unm4+(pres[ip4jk/(nspec+5)]*unp4-pres[im4jk/(nspec+5)]*unm4)))/dx[0];
                        
                        for(int nsp=I_SP;nsp<nspec+5;nsp++){
                            flux[fijk+nsp]-=
                            (ALPHA*(cons[ip1jk+nsp]*unp1-cons[im1jk+nsp]*unm1)
                            +BETA *(cons[ip2jk+nsp]*unp2-cons[im2jk+nsp]*unm2)
                            +GAMMA*(cons[ip3jk+nsp]*unp3-cons[im3jk+nsp]*unm3)
                            +DELTA*(cons[ip4jk+nsp]*unp4-cons[im4jk+nsp]*unm4))/dx[0];
                        }

                        int ijp1k=k*z4d_offset+(j+1)*y4d_offset+i*x4d_offset;
                        int ijp2k=k*z4d_offset+(j+2)*y4d_offset+i*x4d_offset;
                        int ijp3k=k*z4d_offset+(j+3)*y4d_offset+i*x4d_offset;
                        int ijp4k=k*z4d_offset+(j+4)*y4d_offset+i*x4d_offset;
                        
                        int ijm1k=k*z4d_offset+(j-1)*y4d_offset+i*x4d_offset;
                        int ijm2k=k*z4d_offset+(j-2)*y4d_offset+i*x4d_offset;
                        int ijm3k=k*z4d_offset+(j-3)*y4d_offset+i*x4d_offset;
                        int ijm4k=k*z4d_offset+(j-4)*y4d_offset+i*x4d_offset;

                        unp1 = cons[ijp1k+I_MY]/cons[ijp1k+I_RHO];
                        unp2 = cons[ijp2k+I_MY]/cons[ijp2k+I_RHO];
                        unp3 = cons[ijp3k+I_MY]/cons[ijp3k+I_RHO];
                        unp4 = cons[ijp4k+I_MY]/cons[ijp4k+I_RHO];
                    
                        unm1 = cons[ijm1k+I_MY]/cons[ijm1k+I_RHO];
                        unm2 = cons[ijm2k+I_MY]/cons[ijm2k+I_RHO];
                        unm3 = cons[ijm3k+I_MY]/cons[ijm3k+I_RHO];
                        unm4 = cons[ijm4k+I_MY]/cons[ijm4k+I_RHO];

                        flux[fijk+I_RHO]-=
                        (ALPHA*(cons[ijp1k+I_MY]-cons[ijm1k+I_MY])
                        +BETA *(cons[ijp2k+I_MY]-cons[ijm2k+I_MY])
                        +GAMMA*(cons[ijp3k+I_MY]-cons[ijm3k+I_MY])
                        +DELTA*(cons[ijp4k+I_MY]-cons[ijm4k+I_MY]))/dx[1];

                        flux[fijk+I_MX]-=
                        (ALPHA*(cons[ijp1k+I_MX]*unp1-cons[ijm1k+I_MX]*unm1)
                        +BETA *(cons[ijp2k+I_MX]*unp2-cons[ijm2k+I_MX]*unm2)
                        +GAMMA*(cons[ijp3k+I_MX]*unp3-cons[ijm3k+I_MX]*unm3)
                        +DELTA*(cons[ijp4k+I_MX]*unp4-cons[ijm4k+I_MX]*unm4))/dx[1];

                        flux[fijk+I_MY]-=
                        (ALPHA*(cons[ijp1k+I_MY]*unp1-cons[ijm1k+I_MY]*unm1+(pres[ijp1k/(nspec+5)]-pres[ijm1k/(nspec+5)]))
                        +BETA *(cons[ijp2k+I_MY]*unp2-cons[ijm2k+I_MY]*unm2+(pres[ijp2k/(nspec+5)]-pres[ijm2k/(nspec+5)]))
                        +GAMMA*(cons[ijp3k+I_MY]*unp3-cons[ijm3k+I_MY]*unm3+(pres[ijp3k/(nspec+5)]-pres[ijm3k/(nspec+5)]))
                        +DELTA*(cons[ijp4k+I_MY]*unp4-cons[ijm4k+I_MY]*unm4+(pres[ijp4k/(nspec+5)]-pres[ijm4k/(nspec+5)])))/dx[1];

                        flux[fijk+I_MZ]-=
                        (ALPHA*(cons[ijp1k+I_MZ]*unp1-cons[ijm1k+I_MZ]*unm1)
                        +BETA *(cons[ijp2k+I_MZ]*unp2-cons[ijm2k+I_MZ]*unm2)
                        +GAMMA*(cons[ijp3k+I_MZ]*unp3-cons[ijm3k+I_MZ]*unm3)
                        +DELTA*(cons[ijp4k+I_MZ]*unp4-cons[ijm4k+I_MZ]*unm4))/dx[1];

                        flux[fijk+I_ENE]-=
                        (ALPHA*(cons[ijp1k+I_ENE]*unp1-cons[ijm1k+I_ENE]*unm1+(pres[ijp1k/(nspec+5)]*unp1-pres[ijm1k/(nspec+5)]*unm1))
                        +BETA *(cons[ijp2k+I_ENE]*unp2-cons[ijm2k+I_ENE]*unm2+(pres[ijp2k/(nspec+5)]*unp2-pres[ijm2k/(nspec+5)]*unm2))
                        +GAMMA*(cons[ijp3k+I_ENE]*unp3-cons[ijm3k+I_ENE]*unm3+(pres[ijp3k/(nspec+5)]*unp3-pres[ijm3k/(nspec+5)]*unm3))
                        +DELTA*(cons[ijp4k+I_ENE]*unp4-cons[ijm4k+I_ENE]*unm4+(pres[ijp4k/(nspec+5)]*unp4-pres[ijm4k/(nspec+5)]*unm4)))/dx[1];

                        for(int nsp=I_SP;nsp<nspec+5;nsp++){
                           flux[fijk+nsp]-=
                           (ALPHA*(cons[ijp1k+nsp]*unp1-cons[ijm1k+nsp]*unm1)
                           +BETA *(cons[ijp2k+nsp]*unp2-cons[ijm2k+nsp]*unm2)
                           +GAMMA*(cons[ijp3k+nsp]*unp3-cons[ijm3k+nsp]*unm3)
                           +DELTA*(cons[ijp4k+nsp]*unp4-cons[ijm4k+nsp]*unm4))/dx[1];
                        }

                        int ijkp1=(k+1)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        int ijkp2=(k+2)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        int ijkp3=(k+3)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        int ijkp4=(k+4)*z4d_offset+j*y4d_offset+i*x4d_offset;
                                       
                        int ijkm1=(k-1)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        int ijkm2=(k-2)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        int ijkm3=(k-3)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        int ijkm4=(k-4)*z4d_offset+j*y4d_offset+i*x4d_offset;
                        
                        unp1 = cons[ijkp1+I_MZ]/cons[ijkp1+I_RHO];
                        unp2 = cons[ijkp2+I_MZ]/cons[ijkp2+I_RHO];
                        unp3 = cons[ijkp3+I_MZ]/cons[ijkp3+I_RHO];
                        unp4 = cons[ijkp4+I_MZ]/cons[ijkp4+I_RHO];

                        unm1 = cons[ijkm1+I_MZ]/cons[ijkm1+I_RHO];
                        unm2 = cons[ijkm2+I_MZ]/cons[ijkm2+I_RHO];
                        unm3 = cons[ijkm3+I_MZ]/cons[ijkm3+I_RHO];
                        unm4 = cons[ijkm4+I_MZ]/cons[ijkm4+I_RHO];

                        flux[fijk+I_RHO]-=
                        (ALPHA*(cons[ijkp1+I_MZ]-cons[ijkm1+I_MZ])
                        +BETA *(cons[ijkp2+I_MZ]-cons[ijkm2+I_MZ])
                        +GAMMA*(cons[ijkp3+I_MZ]-cons[ijkm3+I_MZ])
                        +DELTA*(cons[ijkp4+I_MZ]-cons[ijkm4+I_MZ]))/dx[2];

                        flux[fijk+I_MX]-=
                        (ALPHA*(cons[ijkp1+I_MX]*unp1-cons[ijkm1+I_MX]*unm1)
                        +BETA *(cons[ijkp2+I_MX]*unp2-cons[ijkm2+I_MX]*unm2)
                        +GAMMA*(cons[ijkp3+I_MX]*unp3-cons[ijkm3+I_MX]*unm3)
                        +DELTA*(cons[ijkp4+I_MX]*unp4-cons[ijkm4+I_MX]*unm4))/dx[2];

                        flux[fijk+I_MY]-=
                        (ALPHA*(cons[ijkp1+I_MY]*unp1-cons[ijkm1+I_MY]*unm1)
                        +BETA *(cons[ijkp2+I_MY]*unp2-cons[ijkm2+I_MY]*unm2)
                        +GAMMA*(cons[ijkp3+I_MY]*unp3-cons[ijkm3+I_MY]*unm3)
                        +DELTA*(cons[ijkp4+I_MY]*unp4-cons[ijkm4+I_MY]*unm4))/dx[2];

                        flux[fijk+I_MZ]-=
                        (ALPHA*(cons[ijkp1+I_MZ]*unp1-cons[ijkm1+I_MZ]*unm1+(pres[ijkp1/(nspec+5)]-pres[ijkm1/(nspec+5)]))
                        +BETA *(cons[ijkp2+I_MZ]*unp2-cons[ijkm2+I_MZ]*unm2+(pres[ijkp2/(nspec+5)]-pres[ijkm2/(nspec+5)]))
                        +GAMMA*(cons[ijkp3+I_MZ]*unp3-cons[ijkm3+I_MZ]*unm3+(pres[ijkp3/(nspec+5)]-pres[ijkm3/(nspec+5)]))
                        +DELTA*(cons[ijkp4+I_MZ]*unp4-cons[ijkm4+I_MZ]*unm4+(pres[ijkp4/(nspec+5)]-pres[ijkm4/(nspec+5)])))/dx[2];

                        flux[fijk+I_ENE]-=
                        (ALPHA*(cons[ijkp1+I_ENE]*unp1-cons[ijkm1+I_ENE]*unm1+(pres[ijkp1/(nspec+5)]*unp1-pres[ijkm1/(nspec+5)]*unm1))
                        +BETA *(cons[ijkp2+I_ENE]*unp2-cons[ijkm2+I_ENE]*unm2+(pres[ijkp2/(nspec+5)]*unp2-pres[ijkm2/(nspec+5)]*unm2))
                        +GAMMA*(cons[ijkp3+I_ENE]*unp3-cons[ijkm3+I_ENE]*unm3+(pres[ijkp3/(nspec+5)]*unp3-pres[ijkm3/(nspec+5)]*unm3))
                        +DELTA*(cons[ijkp4+I_ENE]*unp4-cons[ijkm4+I_ENE]*unm4+(pres[ijkp4/(nspec+5)]*unp4-pres[ijkm4/(nspec+5)]*unm4)))/dx[2];

                        for(int nsp=I_SP;nsp<nspec+5;nsp++){
                           flux[fijk+nsp]-=
                           (ALPHA*(cons[ijkp1+nsp]*unp1-cons[ijkm1+nsp]*unm1)
                           +BETA *(cons[ijkp2+nsp]*unp2-cons[ijkm2+nsp]*unm2)
                           +GAMMA*(cons[ijkp3+nsp]*unp3-cons[ijkm3+nsp]*unm3)
                           +DELTA*(cons[ijkp4+nsp]*unp4-cons[ijkm4+nsp]*unm4))/dx[2];
                        }
                    }
                }
            }
        }
    }
}


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

//void *hypterm_threaded_ispc(void *threadarg){
//    struct thread_data *calc_params = (struct thread_data *) threadarg;
//    int thread_id = calc_params->thread_id;
//    int n[3]={calc_params->n0,calc_params->n1,calc_params->n2};
//    int ns[3]={calc_params->n0s,calc_params->n1s,calc_params->n2s};
//    int ne[3]={calc_params->n0e,calc_params->n1e,calc_params->n2e};
//    int ng[3]={calc_params->ng0,calc_params->ng1,calc_params->ng2};
//    double dx[3]={calc_params->dx0,calc_params->dx1,calc_params->dx2};
//    int blocksize=calc_params->blocksize;
//
//    int nspec=calc_params->nspec;
//    double *pres=calc_params->pres;
//    double *cons=calc_params->cons;
//    double *flux=calc_params->flux;
//    ispc::hypterm_ispc(n,ns,ne,ng,dx,nspec,cons,pres,flux,blocksize);
//}
