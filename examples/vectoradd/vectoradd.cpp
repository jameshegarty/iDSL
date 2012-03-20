#include <pthread.h>

#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

struct thread_data{
    int thread_id;
    double *a;
    double *b;
    double *c;
    long n0;
    long n1;
    double runtime;
};

void *vectoradd_threaded(void *threadarg){
    struct thread_data *vectoradd_params=(struct thread_data *) threadarg;
    int thread_id=vectoradd_params->thread_id;
    double *a=vectoradd_params->a;
    double *b=vectoradd_params->b;
    double *c=vectoradd_params->c;
    long n0=vectoradd_params->n0;
    long n1=vectoradd_params->n1;
    
    for(long i=n0;i<n1;i++){
        a[i]=1.0;
        b[i]=2.0;
        c[i]=3.0;
    }
    for(long i=n0;i<n1;i++){
        c[i]=a[i]+b[i];
    }
    for(long i=n0;i<n1;i++){
        a[i]=4.0;
        b[i]=5.0;
        c[i]=6.0;
    }

    timespec start;
    timespec end;
    clock_gettime(CLOCK_REALTIME,&start);
    for(long i=n0;i<n1;i++){
        c[i]=a[i]+b[i];
    }
    clock_gettime(CLOCK_REALTIME,&end);
    timespec diff;
    if((end.tv_nsec-start.tv_nsec)<0){
        diff.tv_sec=end.tv_sec-start.tv_sec-1;
        diff.tv_nsec=1000000000+end.tv_nsec-start.tv_nsec;
    }
    else{
        diff.tv_sec=end.tv_sec-start.tv_sec;
        diff.tv_nsec=end.tv_nsec-start.tv_nsec;
    }
    vectoradd_params->runtime=((double)diff.tv_sec + ((double)diff.tv_nsec/1E9));
    //std::cout << "[" << thread_id << "]" << "Runtime = " << vectoradd_params->runtime << std::endl;

}
int main(int argc,char **argv){
    if(argc!=3){
        std::cout<<"Usage: <executable> <number_of_threads> <size>"<<std::endl;
        return EXIT_FAILURE;
    }
    int numthreads=atoi(argv[1]);
    const long long SIZE=atoll(argv[2]);
    double *a=new double[SIZE];
    double *b=new double[SIZE];
    double *c=new double[SIZE];

    pthread_t threads[numthreads];
    struct thread_data thread_data_array[numthreads];
    void *status;
    std::stringstream thread_init_ss;
    thread_init_ss << "Creating threads:";
    for(int t=0;t<numthreads;t++){
        thread_data_array[t].thread_id=t;
        thread_data_array[t].a=a;
        thread_data_array[t].b=b;
        thread_data_array[t].c=c;
        thread_data_array[t].n0=(long)((((double)(t))/numthreads)*SIZE);
        thread_data_array[t].n1=(long)((((double)(t+1))/numthreads)*SIZE);
        thread_init_ss<<" "<<t<<"["<<thread_data_array[t].n0<<","<<thread_data_array[t].n1<<"]";
        int rc=pthread_create(&threads[t],NULL,vectoradd_threaded,(void*)&thread_data_array[t]);
        if(rc){
            std::cout<<"ERROR; return code from pthread_create() is"<<rc<<std::endl;
            return EXIT_FAILURE;
        }
    }
    std::cout<<thread_init_ss.str()<<std::endl;
    for(int t=0;t<numthreads;t++){
        pthread_join(threads[t],&status);
    }

    double total_runtime=0.0;
    for(int t=0;t<numthreads;t++){
        total_runtime+=thread_data_array[t].runtime;
    }
    std::cout<<"Threads, Length, Avg. Runtime = "<<numthreads<<", "<<SIZE<<", "<<total_runtime/numthreads<<std::endl;

    delete[] a;
    delete[] b;
    delete[] c;

    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
