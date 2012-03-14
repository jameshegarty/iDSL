#include <pthread.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "constants.h"
#include "hypterm.h"
//#include "hypterm_ispc.h"

void init(int *n,int *ng,double *dx,int nspec,double *cons,double *pres){
    double scale[3]={0.02,0.02,0.02};
    int cons_length=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0])*(nspec+5);
    int pres_length=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int z4d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(nspec+5);
    int y4d_offset=(n[1]+2*ng[1])*(nspec+5);
    int x4d_offset=(nspec+5);
    int s4d_offset=1;
    int z3d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1]);
    int y3d_offset=(n[1]+2*ng[1]);
    int x3d_offset=1;

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
                
                int ijk4d=k*z4d_offset+j*y4d_offset+i*x4d_offset;
                int ijk3d=k*z3d_offset+j*y3d_offset+i*x3d_offset;
                cons[ijk4d+I_RHO]=rholoc;
                cons[ijk4d+I_MX]=rholoc*uvel;
                cons[ijk4d+I_MY]=rholoc*vvel;
                cons[ijk4d+I_MZ]=rholoc*wvel;
                cons[ijk4d+I_ENE]=ploc/0.4+rholoc*(uvel*uvel+vvel*vvel+wvel*wvel)/2.0;

                pres[ijk3d]=ploc;

                cons[ijk4d+I_SP]=0.2+0.1*uvel;
                cons[ijk4d+I_SP+1]=0.2+0.05*vvel;
                cons[ijk4d+I_SP+2]=0.2+0.03*wvel;
                cons[ijk4d+I_SP+3]=1.0-cons[ijk4d+I_SP]-cons[ijk4d+I_SP+1]-cons[ijk4d+I_SP+2];
            }
        }
    }
}

int main(int argc,char **argv){
    if(argc < 3){
        std::cout << "Usage: <executable> <number_of_threads> <blocksize>" << std::endl;
        exit(EXIT_FAILURE);
    }
    int numthreads=atoi(argv[1]);
    int blocksize=atoi(argv[2]);

    std::cout << "Block Size (Double Words)=" << blocksize << std::endl;
    std::cout << "Number of Threads=" << numthreads << std::endl;

    int nspec=9;
    double dx[3]={1e-3,1e-3,1e-3};
    
    int n[3]={256,256,256};
    int ng[3]={4,4,4};

    double *pres=new double[(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0])];
    double *cons=new double[(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0])*(nspec+5)];
    double *flux=new double[n[2]*n[1]*n[0]*(nspec+5)];
    double *fluxmag=new double[nspec+5];

    timespec start_threaded;
    timespec start_serial;
    timespec start_ispc;
    timespec start_threaded_ispc;
    timespec end_threaded;
    timespec end_serial;
    timespec end_ispc;
    timespec end_threaded_ispc;
    init(n,ng,dx,nspec,cons,pres);
    
    ////THREADED
    //for(int m=0;m<n[2]*n[1]*n[0]*(nspec+5);m++){
    //    flux[m]=0.0;
    //}
    //for(int m=0;m<nspec+5;m++){
    //    fluxmag[m]=0.0;
    //}
    //clock_gettime(CLOCK_REALTIME,&start_threaded);
    //pthread_t threads[numthreads];
    //struct thread_data thread_data_array[numthreads];
    //void *status;
    //std::stringstream thread_init_ss;
    //thread_init_ss << "Creating threads:";
    //for(int t=0;t<numthreads;t++){
    //    thread_data_array[t].thread_id=t;
    //    thread_data_array[t].n0=n[0];
    //    thread_data_array[t].n1=n[1];
    //    thread_data_array[t].n2=n[2];
    //    //thread_data_array[t].n0s=(t%4)*(n[0]/4);
    //    //thread_data_array[t].n1s=((t/4)%4)*(n[1]/4);
    //    //thread_data_array[t].n2s=((t/16)%2)*(n[2]/2);
    //    //thread_data_array[t].n0e=((t%4)+1)*(n[0]/4);
    //    //thread_data_array[t].n1e=(((t/4)%4)+1)*(n[1]/4);
    //    //thread_data_array[t].n2e=(((t/16)%2)+1)*(n[2]/2);
    //    thread_data_array[t].n0s=0;
    //    thread_data_array[t].n1s=0;
    //    thread_data_array[t].n2s=0;
    //    thread_data_array[t].n0e=n[0];
    //    thread_data_array[t].n1e=n[1];
    //    thread_data_array[t].n2e=n[2];
    //    thread_data_array[t].ng0=ng[0];
    //    thread_data_array[t].ng1=ng[1];
    //    thread_data_array[t].ng2=ng[2];
    //    thread_data_array[t].dx0=dx[0];
    //    thread_data_array[t].dx1=dx[1];
    //    thread_data_array[t].dx2=dx[2];
    //    thread_data_array[t].nspec=nspec;
    //    thread_data_array[t].pres=pres;
    //    thread_data_array[t].cons=cons;
    //    thread_data_array[t].flux=flux;
    //    thread_data_array[t].blocksize=blocksize;
    //    thread_init_ss << " " << t;
    //    int rc=pthread_create(&threads[t],NULL,hypterm_threaded,(void*)&thread_data_array[t]);
    //    if(rc){
    //        std::cout << "ERROR; return code from pthread_create() is " << rc << std::endl;
    //        exit(EXIT_FAILURE);
    //    }
    //}
    //std::cout << thread_init_ss.str() << std::endl;
    //
    //for(int t=0;t<numthreads;t++){
    //    pthread_join(threads[t],&status);
    //}
    //clock_gettime(CLOCK_REALTIME,&end_threaded);
    //timespec diff_threaded;
    //diff_threaded.tv_sec=end_threaded.tv_sec-start_threaded.tv_sec;
    //diff_threaded.tv_nsec=end_threaded.tv_nsec-start_threaded.tv_nsec;
    //std::cout << "Threaded Code Runtime: " << ((double)diff_threaded.tv_sec + ((double)diff_threaded.tv_nsec/1E9)) << std::endl;
    //for(int k=0;k<n[2];k++){
    //    for(int j=0;j<n[1];j++){
    //        for(int i=0;i<n[0];i++){
    //            for(int ns=0;ns<nspec+5;ns++){
    //                double fluxelement=flux[k*n[1]*n[0]*(nspec+5)+j*n[0]*(nspec+5)+i*(nspec+5)+ns];
    //                fluxmag[ns]+=fluxelement*fluxelement;
    //            }
    //        }
    //    }
    //}
    //for(int ns=0;ns<nspec+5;ns++){
    //    std::cout<<"threaded: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    //}
    int z4d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(nspec+5);
    int y4d_offset=(n[1]+2*ng[1])*(nspec+5);
    int x4d_offset=(nspec+5);

    //for(int k=0;k<n[2]+2*ng[2];k++){
    //    for(int j=0;j<n[1]+2*ng[1];j++){
    //        for(int i=0;i<n[0]+2*ng[0];i++){
    //            for(int ns=0;ns<nspec+5;ns++){
    //                int ijk4d=k*z4d_offset+j*y4d_offset+i*x4d_offset;
    //                std::cout << std::showpos << std::scientific << std::setprecision(4) << cons[ijk4d+ns] << " ";
    //            }
    //            std::cout << std::endl;
    //        }
    //    }
    //}

    //SERIAL
    for(int m=0;m<n[2]*n[1]*n[0]*(nspec+5);m++){
        flux[m]=0.0;
    }   
    for(int m=0;m<nspec+5;m++){
        fluxmag[m]=0.0;
    }
    clock_gettime(CLOCK_REALTIME,&start_serial);
    int n0[3]={0,0,0};
    hypterm_serial(n,n0,n,ng,dx,nspec,cons,pres,flux,blocksize);
    clock_gettime(CLOCK_REALTIME,&end_serial);
    timespec diff_serial;
    diff_serial.tv_sec=end_serial.tv_sec-start_serial.tv_sec;
    diff_serial.tv_nsec=end_serial.tv_nsec-start_serial.tv_nsec;
    std::cout << "Serial Code Runtime: " << ((double)diff_serial.tv_sec + ((double)diff_serial.tv_nsec/1E9)) << std::endl;
    for(int k=0;k<n[2];k++){
        for(int j=0;j<n[1];j++){
            for(int i=0;i<n[0];i++){
                for(int ns=0;ns<nspec+5;ns++){
                    double fluxelement=flux[k*n[1]*n[0]*(nspec+5)+j*n[0]*(nspec+5)+i*(nspec+5)+ns];
                    fluxmag[ns]+=fluxelement*fluxelement;
                }
            }
        }
    }
    for(int ns=0;ns<nspec+5;ns++){
        std::cout<<"serial: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    delete[] pres;
    std::cout << "after delete pres" << std::endl;
    delete[] cons;
    std::cout << "after delete cons" << std::endl;
    delete[] flux;
    std::cout << "after delete flux" << std::endl;
    delete[] fluxmag;
    std::cout << "after delete fluxmag" << std::endl;
    
    //pthread_exit(NULL);
    return EXIT_SUCCESS;
}
