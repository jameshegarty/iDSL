#include <pthread.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "constants.h"
#include "hypterm.h"
#include "hypterm_ispc.h"

void init(int *n,int *ng,double *dx,int nspec,double *cons,double *pres){
    double scale[3]={0.02,0.02,0.02};
    int cons_length=(nspec+5)*(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int pres_length=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    
    int s_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int z_offset=(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int y_offset=(n[0]+2*ng[0]);
    int x_offset=1;

    for(int m=0;m<cons_length;m++){
        cons[m]=0.0;
    }
    for(int m=0;m<pres_length;m++){
        pres[m]=0.0;
    }

    for(int k=0;k<n[2]+2*ng[2];k++){
        double zloc=(k-ng[2]+1)*dx[2]/scale[2];
        for(int j=0;j<n[1]+2*ng[1];j++){
            double yloc=(j-ng[1]+1)*dx[1]/scale[1];
            for(int i=0;i<n[0]+2*ng[0];i++){
                double xloc=(i-ng[0]+1)*dx[0]/scale[0];

                double uvel=sin(xloc)*sin(2.0*yloc)*sin(3.0*zloc);
                double vvel=sin(2.0*xloc)*sin(4.0*yloc)*sin(zloc);
                double wvel=sin(3.0*xloc)*cos(2.0*yloc)*sin(2.0*zloc);
                double rholoc=1e-3+1e-5*sin(xloc)*cos(2.0*yloc)*cos(3.0*zloc);
                double ploc=1e6+1e-3*sin(2.0*xloc)*cos(2.0*yloc)*sin(2.0*zloc);
                
                int ijk=k*z_offset+j*y_offset+i*x_offset;

                cons[ijk+s_offset*I_RHO]=rholoc;
                cons[ijk+s_offset*I_MX]=rholoc*uvel;
                cons[ijk+s_offset*I_MY]=rholoc*vvel;
                cons[ijk+s_offset*I_MZ]=rholoc*wvel;
                cons[ijk+s_offset*I_ENE]=ploc/0.4+rholoc*(uvel*uvel+vvel*vvel+wvel*wvel)/2.0;

                pres[ijk]=ploc;

                cons[ijk+s_offset*I_SP]=0.2+0.1*uvel;
                cons[ijk+s_offset*(I_SP+1)]=0.2+0.05*vvel;
                cons[ijk+s_offset*(I_SP+2)]=0.2+0.03*wvel;
                cons[ijk+s_offset*(I_SP+3)]=1.0-cons[ijk+s_offset*I_SP]-cons[ijk+s_offset*(I_SP+1)]-cons[ijk+s_offset*(I_SP+2)];
            }
        }
    }
}

int main(int argc,const char **argv){
    if(argc < 4){
        std::cout << "Usage: <executable> <blocksizeL1> <blocksizeL2> <blocksizeL3>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int blocksizeL1=atoi(argv[1]);
    int blocksizeL2=atoi(argv[2]);
    int blocksizeL3=atoi(argv[3]);

    std::cout << "Block Sizes (L1, L2, L3 -- Double Words)=" << blocksizeL1 << "," << blocksizeL2 << "," << blocksizeL3 << std::endl;

    int nspec=9;
    double dx[3]={1e-3,1e-3,1e-3};
    
    int n0[3]={0,0,0};
    int n[3]={64,64,64};
    int ng[3]={4,4,4};

    double *pres=new double[(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0])];
    double *cons=new double[(nspec+5)*(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0])];
    double *flux=new double[(nspec+5)*n[2]*n[1]*n[0]];
    double *fluxmag=new double[nspec+5];

    timespec start_serial;
    timespec start_ispc;
    timespec end_serial;
    timespec end_ispc;
    init(n,ng,dx,nspec,cons,pres);
    
    int s_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int z_offset=(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int y_offset=(n[0]+2*ng[0]);
    int x_offset=1;

    //SERIAL
    for(int m=0;m<n[2]*n[1]*n[0]*(nspec+5);m++){
        flux[m]=0.0;
    }   
    for(int m=0;m<nspec+5;m++){
        fluxmag[m]=0.0;
    }
    clock_gettime(CLOCK_REALTIME,&start_serial);
    hypterm_serial(n,n0,n,ng,dx,nspec,cons,pres,flux,blocksizeL1,blocksizeL2,blocksizeL3);
    clock_gettime(CLOCK_REALTIME,&end_serial);
    timespec diff_serial;
    diff_serial.tv_sec=end_serial.tv_sec-start_serial.tv_sec;
    diff_serial.tv_nsec=end_serial.tv_nsec-start_serial.tv_nsec;
    std::cout << "Serial Code Runtime: " << ((double)diff_serial.tv_sec + ((double)diff_serial.tv_nsec/1E9)) << std::endl;
    for(int ns=0;ns<nspec+5;ns++){
        for(int k=0;k<n[2];k++){
            for(int j=0;j<n[1];j++){
                for(int i=0;i<n[0];i++){
                    double fluxelement=flux[ns*n[2]*n[1]*n[0]+k*n[1]*n[0]+j*n[0]+i];
                    fluxmag[ns]+=fluxelement*fluxelement;
                }
            }
        }
    }
    for(int ns=0;ns<nspec+5;ns++){
        std::cout<<"serial: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    //SERIAL ISPC
    for(int m=0;m<n[2]*n[1]*n[0]*(nspec+5);m++){
        flux[m]=0.0;
    }   
    for(int m=0;m<nspec+5;m++){
        fluxmag[m]=0.0;
    }
    clock_gettime(CLOCK_REALTIME,&start_ispc);
    ispc::hypterm_ispc(n,n0,n,ng,dx,nspec,cons,pres,flux);
    clock_gettime(CLOCK_REALTIME,&end_ispc);
    timespec diff_ispc;
    diff_ispc.tv_sec=end_ispc.tv_sec-start_ispc.tv_sec;
    diff_ispc.tv_nsec=end_ispc.tv_nsec-start_ispc.tv_nsec;
    std::cout << "ispc Code Runtime: " << ((double)diff_ispc.tv_sec + ((double)diff_ispc.tv_nsec/1E9)) << std::endl;
    for(int ns=0;ns<nspec+5;ns++){
        for(int k=0;k<n[2];k++){
            for(int j=0;j<n[1];j++){
                for(int i=0;i<n[0];i++){
                    double fluxelement=flux[ns*n[2]*n[1]*n[0]+k*n[1]*n[0]+j*n[0]+i];
                    fluxmag[ns]+=fluxelement*fluxelement;
                }
            }
        }
    }
    for(int ns=0;ns<nspec+5;ns++){
        std::cout<<"ispc: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    delete[] pres;
    delete[] cons;
    delete[] flux;
    delete[] fluxmag;
    
    //pthread_exit(NULL);
    return EXIT_SUCCESS;
}
