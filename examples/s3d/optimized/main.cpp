#include <pthread.h>

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "constants.h"
#include "hypterm.h"
#include "hypterm_ispc.h"

void init(int *n,int *ng,double *dx,int nspec,double *cons,double *pres){
    double scale[3]={0.02,0.02,0.02};

    int offset0=(n[0]+2*ng[0])*(n[1]+2*ng[1])*(n[2]+2*ng[2]);
    int offset1=(n[1]+2*ng[1])*(n[2]+2*ng[2]);
    int offset2=(n[2]+2*ng[2]);

    for(int i=I_SP*offset0;i<nspec*offset0;i++){
        cons[i]=0.0;
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
                
                int ijk4d=i*offset1+j*offset2+k;
                cons[I_RHO*offset0+ijk4d]=rholoc;
                cons[I_MX*offset0+ijk4d]=rholoc*uvel;
                cons[I_MY*offset0+ijk4d]=rholoc*vvel;
                cons[I_MZ*offset0+ijk4d]=rholoc*wvel;
                cons[I_ENE*offset0+ijk4d]=ploc/0.4+rholoc*(uvel*uvel+vvel*vvel+wvel*wvel)/2.0;

                pres[ijk4d]=ploc;

                cons[I_SP*offset0+ijk4d]=0.2+0.1*uvel;
                cons[(I_SP+1)*offset0+ijk4d]=0.2+0.05*vvel;
                cons[(I_SP+2)*offset0+ijk4d]=0.2+0.03*wvel;
                cons[(I_SP+3)*offset0+ijk4d]=1.0-cons[I_SP*offset0+ijk4d]-cons[(I_SP+1)*offset0+ijk4d]-cons[(I_SP+2)*offset0+ijk4d];
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

    int nspec=14;
    double dx[3]={1e-3,1e-3,1e-3};
    
    int n[3]={128,128,128};
    int ng[3]={4,4,4};

    double *pres=new double[(n[0]+2*ng[0])*(n[1]+2*ng[1])*(n[2]+2*ng[2])];
    double *cons=new double[nspec*(n[0]+2*ng[0])*(n[1]+2*ng[1])*(n[2]+2*ng[2])];
    double *flux=new double[nspec*n[0]*n[1]*n[2]];
    for(int i=0;i<nspec*n[0]*n[1]*n[2];i++){
        flux[i]=0.0;
    }

    int goffset=n[0]*n[1]*n[2];

    timespec start_threaded;
    timespec start_serial;
    timespec start_ispc;
    timespec end_threaded;
    timespec end_serial;
    timespec end_ispc;
    init(n,ng,dx,nspec,cons,pres);
    clock_gettime(CLOCK_REALTIME,&start_threaded);
    pthread_t threads[numthreads];
    struct thread_data thread_data_array[numthreads];
    void *status;
    for(int t=0;t<numthreads;t++){
        thread_data_array[t].thread_id=t;
        thread_data_array[t].n0=n[0];
        thread_data_array[t].n1=n[1];
        thread_data_array[t].n2=n[2];
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
    

    double fluxmag[nspec];
    for(int i=0;i<nspec;i++){
        fluxmag[i]=0.0;
    }

    for(int ns=0;ns<nspec;ns++){
        for(int i=0;i<n[0];i++){
            for(int j=0;j<n[1];j++){
                for(int k=0;k<n[2];k++){
                    fluxmag[ns]=fluxmag[ns]+flux[ns*goffset+i*n[1]*n[2]+j*n[2]+k]*flux[ns*goffset+i*n[1]*n[2]+j*n[2]+k];
                }
            }
        }
    }
    
    for(int ns=0;ns<nspec;ns++){
        std::cout<<"threaded: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    for(int i=0;i<nspec*n[0]*n[1]*n[2];i++){
        flux[i]=0.0;
    }   
    
    for(int i=0;i<nspec;i++){
        fluxmag[i]=0.0;
    }

    clock_gettime(CLOCK_REALTIME,&start_serial);
    int n0[3]={0,0,0};
    hypterm_serial(n,n0,n,ng,dx,nspec,cons,pres,flux,blocksize);
    clock_gettime(CLOCK_REALTIME,&end_serial);
    timespec diff_serial;
    diff_serial.tv_sec=end_serial.tv_sec-start_serial.tv_sec;
    diff_serial.tv_nsec=end_serial.tv_nsec-start_serial.tv_nsec;
    std::cout << "Serial Code Runtime: " << ((double)diff_serial.tv_sec + ((double)diff_serial.tv_nsec/1E9)) << std::endl;

    for(int ns=0;ns<nspec;ns++){
        for(int i=0;i<n[0];i++){
            for(int j=0;j<n[1];j++){
                for(int k=0;k<n[2];k++){
                    fluxmag[ns]=fluxmag[ns]+flux[ns*goffset+i*n[1]*n[2]+j*n[2]+k]*flux[ns*goffset+i*n[1]*n[2]+j*n[2]+k];
                }
            }
        }
    }
    
    for(int ns=0;ns<nspec;ns++){
        std::cout<<"serial: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    for(int i=0;i<nspec*n[0]*n[1]*n[2];i++){
        flux[i]=0.0;
    }   
    
    for(int i=0;i<nspec;i++){
        fluxmag[i]=0.0;
    }

    clock_gettime(CLOCK_REALTIME,&start_ispc);
    ispc::hypterm_ispc(n,n0,n,ng,dx,nspec,cons,pres,flux,blocksize);
    clock_gettime(CLOCK_REALTIME,&end_ispc);
    timespec diff_ispc;
    diff_ispc.tv_sec=end_ispc.tv_sec-start_ispc.tv_sec;
    diff_ispc.tv_nsec=end_ispc.tv_nsec-start_ispc.tv_nsec;
    std::cout << "ispc Code Runtime: " << ((double)diff_ispc.tv_sec + ((double)diff_ispc.tv_nsec/1E9)) << std::endl;

    for(int ns=0;ns<nspec;ns++){
        for(int i=0;i<n[0];i++){
            for(int j=0;j<n[1];j++){
                for(int k=0;k<n[2];k++){
                    fluxmag[ns]=fluxmag[ns]+flux[ns*goffset+i*n[1]*n[2]+j*n[2]+k]*flux[ns*goffset+i*n[1]*n[2]+j*n[2]+k];
                }
            }
        }
    }
    
    for(int ns=0;ns<nspec;ns++){
        std::cout<<"ispc: component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
    }

    delete[] pres;
    delete[] cons;
    delete[] flux;
    
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
