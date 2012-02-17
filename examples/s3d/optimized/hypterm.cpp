#include "hypterm.h"

void hypterm_serial(int *n,int *ns,int *ne,int *ng,double *dx,int nspec,double *cons,double *pres,double *flux,int blocksize){
    for(int ii=ns[0]+ng[0];ii<ne[0]+ng[0];ii+=blocksize){
        for(int jj=ns[1]+ng[1];jj<ne[1]+ng[1];jj+=blocksize){
            for(int kk=ns[2]+ng[2];kk<ne[2]+ng[2];kk+=blocksize){
                for(int i=ii;i<std::min(ii+blocksize,ne[0]+ng[0]);i++){
                    for(int j=jj;j<std::min(jj+blocksize,ne[1]+ng[1]);j++){
                        for(int k=kk;k<std::min(kk+blocksize,ne[2]+ng[2]);k++){
                            int offset0=(n[0]+2*ng[0])*(n[1]+2*ng[1])*(n[2]+2*ng[2]);
                            int offset1=(n[1]+2*ng[1])*(n[2]+2*ng[2]);
                            int offset2=(n[2]+2*ng[2]);

                            int goffset=n[0]*n[1]*n[2];
                            int gijk=(i-ng[0])*n[1]*n[2]+(j-ng[1])*n[2]+(k-ng[2]);

                            int ip1jk=(i+1)*offset1+j*offset2+k;
                            int ip2jk=(i+2)*offset1+j*offset2+k;
                            int ip3jk=(i+3)*offset1+j*offset2+k;
                            int ip4jk=(i+4)*offset1+j*offset2+k;

                            int im1jk=(i-1)*offset1+j*offset2+k;
                            int im2jk=(i-2)*offset1+j*offset2+k;
                            int im3jk=(i-3)*offset1+j*offset2+k;
                            int im4jk=(i-4)*offset1+j*offset2+k;
                            
                            double unp1=cons[I_MX*offset0+ip1jk]/cons[I_RHO*offset0+ip1jk];
                            double unp2=cons[I_MX*offset0+ip2jk]/cons[I_RHO*offset0+ip2jk];
                            double unp3=cons[I_MX*offset0+ip3jk]/cons[I_RHO*offset0+ip3jk];
                            double unp4=cons[I_MX*offset0+ip4jk]/cons[I_RHO*offset0+ip4jk];
                                                                                          
                            double unm1=cons[I_MX*offset0+im1jk]/cons[I_RHO*offset0+im1jk];
                            double unm2=cons[I_MX*offset0+im2jk]/cons[I_RHO*offset0+im2jk];
                            double unm3=cons[I_MX*offset0+im3jk]/cons[I_RHO*offset0+im3jk];
                            double unm4=cons[I_MX*offset0+im4jk]/cons[I_RHO*offset0+im4jk];

                            flux[I_RHO*goffset+gijk]=flux[I_RHO*goffset+gijk]-
                            (ALPHA*(cons[I_MX*offset0+ip1jk]-cons[I_MX*offset0+im1jk])
                            +BETA *(cons[I_MX*offset0+ip2jk]-cons[I_MX*offset0+im2jk])
                            +GAMMA*(cons[I_MX*offset0+ip3jk]-cons[I_MX*offset0+im3jk])
                            +DELTA*(cons[I_MX*offset0+ip4jk]-cons[I_MX*offset0+im4jk]))/dx[0];
                            
                            flux[I_MX*goffset+gijk]=flux[I_MX*goffset+gijk]-
                            (ALPHA*(cons[I_MX*offset0+ip1jk]*unp1-cons[I_MX*offset0+im1jk]*unm1+(pres[ip1jk]-pres[im1jk]))
                            +BETA *(cons[I_MX*offset0+ip2jk]*unp2-cons[I_MX*offset0+im2jk]*unm2+(pres[ip2jk]-pres[im2jk]))
                            +GAMMA*(cons[I_MX*offset0+ip3jk]*unp3-cons[I_MX*offset0+im3jk]*unm3+(pres[ip3jk]-pres[im3jk]))
                            +DELTA*(cons[I_MX*offset0+ip4jk]*unp4-cons[I_MX*offset0+im4jk]*unm4+(pres[ip4jk]-pres[im4jk])))/dx[0];

                            flux[I_MY*goffset+gijk]=flux[I_MY*goffset+gijk]-
                            (ALPHA*(cons[I_MY*offset0+ip1jk]*unp1-cons[I_MY*offset0+im1jk]*unm1)
                            +BETA *(cons[I_MY*offset0+ip2jk]*unp2-cons[I_MY*offset0+im2jk]*unm2)
                            +GAMMA*(cons[I_MY*offset0+ip3jk]*unp3-cons[I_MY*offset0+im3jk]*unm3)
                            +DELTA*(cons[I_MY*offset0+ip4jk]*unp4-cons[I_MY*offset0+im4jk]*unm4))/dx[0];

                            flux[I_MZ*goffset+gijk]=flux[I_MZ*goffset+gijk]-
                            (ALPHA*(cons[I_MZ*offset0+ip1jk]*unp1-cons[I_MZ*offset0+im1jk]*unm1)
                            +BETA *(cons[I_MZ*offset0+ip2jk]*unp2-cons[I_MZ*offset0+im2jk]*unm2)
                            +GAMMA*(cons[I_MZ*offset0+ip3jk]*unp3-cons[I_MZ*offset0+im3jk]*unm3)
                            +DELTA*(cons[I_MZ*offset0+ip4jk]*unp4-cons[I_MZ*offset0+im4jk]*unm4))/dx[0];

                            flux[I_ENE*goffset+gijk]=flux[I_ENE*goffset+gijk]-
                            (ALPHA*(cons[I_ENE*offset0+ip1jk]*unp1-cons[I_ENE*offset0+im1jk]*unm1+(pres[ip1jk]*unp1-pres[im1jk]*unm1))
                            +BETA *(cons[I_ENE*offset0+ip2jk]*unp2-cons[I_ENE*offset0+im2jk]*unm2+(pres[ip2jk]*unp2-pres[im2jk]*unm2))
                            +GAMMA*(cons[I_ENE*offset0+ip3jk]*unp3-cons[I_ENE*offset0+im3jk]*unm3+(pres[ip3jk]*unp3-pres[im3jk]*unm3))
                            +DELTA*(cons[I_ENE*offset0+ip4jk]*unp4-cons[I_ENE*offset0+im4jk]*unm4+(pres[ip4jk]*unp4-pres[im4jk]*unm4)))/dx[0];
                            
                            for(int nsp=I_SP;nsp<nspec;nsp++){
                                flux[nsp*goffset+gijk]=flux[nsp*goffset+gijk]-
                                (ALPHA*(cons[nsp*offset0+ip1jk]*unp1-cons[nsp*offset0+im1jk]*unm1)
                                +BETA *(cons[nsp*offset0+ip2jk]*unp2-cons[nsp*offset0+im2jk]*unm2)
                                +GAMMA*(cons[nsp*offset0+ip3jk]*unp3-cons[nsp*offset0+im3jk]*unm3)
                                +DELTA*(cons[nsp*offset0+ip4jk]*unp4-cons[nsp*offset0+im4jk]*unm4))/dx[0];
                            }

                            int ijp1k=i*offset1+(j+1)*offset2+k;
                            int ijp2k=i*offset1+(j+2)*offset2+k;
                            int ijp3k=i*offset1+(j+3)*offset2+k;
                            int ijp4k=i*offset1+(j+4)*offset2+k;
                                                     
                            int ijm1k=i*offset1+(j-1)*offset2+k;
                            int ijm2k=i*offset1+(j-2)*offset2+k;
                            int ijm3k=i*offset1+(j-3)*offset2+k;
                            int ijm4k=i*offset1+(j-4)*offset2+k;

                            unp1 = cons[I_MY*offset0+ijp1k]/cons[I_RHO*offset0+ijp1k];
                            unp2 = cons[I_MY*offset0+ijp2k]/cons[I_RHO*offset0+ijp2k];
                            unp3 = cons[I_MY*offset0+ijp3k]/cons[I_RHO*offset0+ijp3k];
                            unp4 = cons[I_MY*offset0+ijp4k]/cons[I_RHO*offset0+ijp4k];
                                                                                 
                            unm1 = cons[I_MY*offset0+ijm1k]/cons[I_RHO*offset0+ijm1k];
                            unm2 = cons[I_MY*offset0+ijm2k]/cons[I_RHO*offset0+ijm2k];
                            unm3 = cons[I_MY*offset0+ijm3k]/cons[I_RHO*offset0+ijm3k];
                            unm4 = cons[I_MY*offset0+ijm4k]/cons[I_RHO*offset0+ijm4k];

                            flux[I_RHO*goffset+gijk]=flux[I_RHO*goffset+gijk]-
                               (ALPHA*(cons[I_MY*offset0+ijp1k]-cons[I_MY*offset0+ijm1k])
                               +BETA *(cons[I_MY*offset0+ijp2k]-cons[I_MY*offset0+ijm2k])
                               +GAMMA*(cons[I_MY*offset0+ijp3k]-cons[I_MY*offset0+ijm3k])
                               +DELTA*(cons[I_MY*offset0+ijp4k]-cons[I_MY*offset0+ijm4k]))/dx[1];

                            flux[I_MX*goffset+gijk]=flux[I_MX*goffset+gijk]-
                               (ALPHA*(cons[I_MX*offset0+ijp1k]*unp1-cons[I_MX*offset0+ijm1k]*unm1)
                               +BETA *(cons[I_MX*offset0+ijp2k]*unp2-cons[I_MX*offset0+ijm2k]*unm2)
                               +GAMMA*(cons[I_MX*offset0+ijp3k]*unp3-cons[I_MX*offset0+ijm3k]*unm3)
                               +DELTA*(cons[I_MX*offset0+ijp4k]*unp4-cons[I_MX*offset0+ijm4k]*unm4))/dx[1];

                            flux[I_MY*goffset+gijk]=flux[I_MY*goffset+gijk]-
                               (ALPHA*(cons[I_MY*offset0+ijp1k]*unp1-cons[I_MY*offset0+ijm1k]*unm1+(pres[ijp1k]-pres[ijm1k]))
                               +BETA *(cons[I_MY*offset0+ijp2k]*unp2-cons[I_MY*offset0+ijm2k]*unm2+(pres[ijp2k]-pres[ijm2k]))
                               +GAMMA*(cons[I_MY*offset0+ijp3k]*unp3-cons[I_MY*offset0+ijm3k]*unm3+(pres[ijp3k]-pres[ijm3k]))
                               +DELTA*(cons[I_MY*offset0+ijp4k]*unp4-cons[I_MY*offset0+ijm4k]*unm4+(pres[ijp4k]-pres[ijm4k])))/dx[1];

                            flux[I_MZ*goffset+gijk]=flux[I_MZ*goffset+gijk]-
                               (ALPHA*(cons[I_MZ*offset0+ijp1k]*unp1-cons[I_MZ*offset0+ijm1k]*unm1)
                               +BETA *(cons[I_MZ*offset0+ijp2k]*unp2-cons[I_MZ*offset0+ijm2k]*unm2)
                               +GAMMA*(cons[I_MZ*offset0+ijp3k]*unp3-cons[I_MZ*offset0+ijm3k]*unm3)
                               +DELTA*(cons[I_MZ*offset0+ijp4k]*unp4-cons[I_MZ*offset0+ijm4k]*unm4))/dx[1];

                            flux[I_ENE*goffset+gijk]=flux[I_ENE*goffset+gijk]-
                               (ALPHA*(cons[I_ENE*offset0+ijp1k]*unp1-cons[I_ENE*offset0+ijm1k]*unm1+(pres[ijp1k]*unp1-pres[ijm1k]*unm1))
                               +BETA *(cons[I_ENE*offset0+ijp2k]*unp2-cons[I_ENE*offset0+ijm2k]*unm2+(pres[ijp2k]*unp2-pres[ijm2k]*unm2))
                               +GAMMA*(cons[I_ENE*offset0+ijp3k]*unp3-cons[I_ENE*offset0+ijm3k]*unm3+(pres[ijp3k]*unp3-pres[ijm3k]*unm3))
                               +DELTA*(cons[I_ENE*offset0+ijp4k]*unp4-cons[I_ENE*offset0+ijm4k]*unm4+(pres[ijp4k]*unp4-pres[ijm4k]*unm4)))/dx[1];

                            for(int nsp=I_SP;nsp<nspec;nsp++){
                               flux[nsp*goffset+gijk]=flux[nsp*goffset+gijk]-
                                  (ALPHA*(cons[nsp*offset0+ijp1k]*unp1-cons[nsp*offset0+ijm1k]*unm1)
                                  +BETA *(cons[nsp*offset0+ijp2k]*unp2-cons[nsp*offset0+ijm2k]*unm2)
                                  +GAMMA*(cons[nsp*offset0+ijp3k]*unp3-cons[nsp*offset0+ijm3k]*unm3)
                                  +DELTA*(cons[nsp*offset0+ijp4k]*unp4-cons[nsp*offset0+ijm4k]*unm4))/dx[1];
                            }

                            int ijkp1=i*offset1+j*offset2+(k+1);
                            int ijkp2=i*offset1+j*offset2+(k+2);
                            int ijkp3=i*offset1+j*offset2+(k+3);
                            int ijkp4=i*offset1+j*offset2+(k+4);
                                                               
                            int ijkm1=i*offset1+j*offset2+(k-1);
                            int ijkm2=i*offset1+j*offset2+(k-2);
                            int ijkm3=i*offset1+j*offset2+(k-3);
                            int ijkm4=i*offset1+j*offset2+(k-4);
                            
                            unp1 = cons[I_MZ*offset0+ijkp1]/cons[I_RHO*offset0+ijkp1];
                            unp2 = cons[I_MZ*offset0+ijkp2]/cons[I_RHO*offset0+ijkp2];
                            unp3 = cons[I_MZ*offset0+ijkp3]/cons[I_RHO*offset0+ijkp3];
                            unp4 = cons[I_MZ*offset0+ijkp4]/cons[I_RHO*offset0+ijkp4];
                                                                                 
                            unm1 = cons[I_MZ*offset0+ijkm1]/cons[I_RHO*offset0+ijkm1];
                            unm2 = cons[I_MZ*offset0+ijkm2]/cons[I_RHO*offset0+ijkm2];
                            unm3 = cons[I_MZ*offset0+ijkm3]/cons[I_RHO*offset0+ijkm3];
                            unm4 = cons[I_MZ*offset0+ijkm4]/cons[I_RHO*offset0+ijkm4];

                            flux[I_RHO*goffset+gijk]=flux[I_RHO*goffset+gijk]-
                               (ALPHA*(cons[I_MZ*offset0+ijkp1]-cons[I_MZ*offset0+ijkm1])
                               +BETA *(cons[I_MZ*offset0+ijkp2]-cons[I_MZ*offset0+ijkm2])
                               +GAMMA*(cons[I_MZ*offset0+ijkp3]-cons[I_MZ*offset0+ijkm3])
                               +DELTA*(cons[I_MZ*offset0+ijkp4]-cons[I_MZ*offset0+ijkm4]))/dx[2];

                            flux[I_MX*goffset+gijk]=flux[I_MX*goffset+gijk]-
                               (ALPHA*(cons[I_MX*offset0+ijkp1]*unp1-cons[I_MX*offset0+ijkm1]*unm1)
                               +BETA *(cons[I_MX*offset0+ijkp2]*unp2-cons[I_MX*offset0+ijkm2]*unm2)
                               +GAMMA*(cons[I_MX*offset0+ijkp3]*unp3-cons[I_MX*offset0+ijkm3]*unm3)
                               +DELTA*(cons[I_MX*offset0+ijkp4]*unp4-cons[I_MX*offset0+ijkm4]*unm4))/dx[2];

                            flux[I_MY*goffset+gijk]=flux[I_MY*goffset+gijk]-
                               (ALPHA*(cons[I_MY*offset0+ijkp1]*unp1-cons[I_MY*offset0+ijkm1]*unm1)
                               +BETA *(cons[I_MY*offset0+ijkp2]*unp2-cons[I_MY*offset0+ijkm2]*unm2)
                               +GAMMA*(cons[I_MY*offset0+ijkp3]*unp3-cons[I_MY*offset0+ijkm3]*unm3)
                               +DELTA*(cons[I_MY*offset0+ijkp4]*unp4-cons[I_MY*offset0+ijkm4]*unm4))/dx[2];

                            flux[I_MZ*goffset+gijk]=flux[I_MZ*goffset+gijk]-
                               (ALPHA*(cons[I_MZ*offset0+ijkp1]*unp1-cons[I_MZ*offset0+ijkm1]*unm1+(pres[ijkp1]-pres[ijkm1]))
                               +BETA *(cons[I_MZ*offset0+ijkp2]*unp2-cons[I_MZ*offset0+ijkm2]*unm2+(pres[ijkp2]-pres[ijkm2]))
                               +GAMMA*(cons[I_MZ*offset0+ijkp3]*unp3-cons[I_MZ*offset0+ijkm3]*unm3+(pres[ijkp3]-pres[ijkm3]))
                               +DELTA*(cons[I_MZ*offset0+ijkp4]*unp4-cons[I_MZ*offset0+ijkm4]*unm4+(pres[ijkp4]-pres[ijkm4])))/dx[2];

                            flux[I_ENE*goffset+gijk]=flux[I_ENE*goffset+gijk]-
                               (ALPHA*(cons[I_ENE*offset0+ijkp1]*unp1-cons[I_ENE*offset0+ijkm1]*unm1+(pres[ijkp1]*unp1-pres[ijkm1]*unm1))
                               +BETA *(cons[I_ENE*offset0+ijkp2]*unp2-cons[I_ENE*offset0+ijkm2]*unm2+(pres[ijkp2]*unp2-pres[ijkm2]*unm2))
                               +GAMMA*(cons[I_ENE*offset0+ijkp3]*unp3-cons[I_ENE*offset0+ijkm3]*unm3+(pres[ijkp3]*unp3-pres[ijkm3]*unm3))
                               +DELTA*(cons[I_ENE*offset0+ijkp4]*unp4-cons[I_ENE*offset0+ijkm4]*unm4+(pres[ijkp4]*unp4-pres[ijkm4]*unm4)))/dx[2];

                            for(int nsp=I_SP;nsp<nspec;nsp++){
                               flux[nsp*goffset+gijk]=flux[nsp*goffset+gijk]-
                                  (ALPHA*(cons[nsp*offset0+ijkp1]*unp1-cons[nsp*offset0+ijkm1]*unm1)
                                  +BETA *(cons[nsp*offset0+ijkp2]*unp2-cons[nsp*offset0+ijkm2]*unm2)
                                  +GAMMA*(cons[nsp*offset0+ijkp3]*unp3-cons[nsp*offset0+ijkm3]*unm3)
                                  +DELTA*(cons[nsp*offset0+ijkp4]*unp4-cons[nsp*offset0+ijkm4]*unm4))/dx[2];
                            }
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
