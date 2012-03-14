export void hypterm_ispc(uniform int n[],uniform int ns[],uniform int ne[],uniform int ng[],uniform double dx[],uniform int nspec,uniform double cons[],uniform double pres[],uniform double flux[],uniform int blocksize){
    const double ALPHA = 0.8;
    const double BETA = -0.2;
    const double GAMMA = 4.0/105.0;
    const double DELTA = -1.0/280.0;
    const int I_RHO = 0;
    const int I_MX = 1;
    const int I_MY = 2;
    const int I_MZ = 3;
    const int I_ENE = 4;
    const int I_SP = 5;

    int z4d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(nspec+5);
    int y4d_offset=(n[1]+2*ng[1])*(nspec+5);
    int x4d_offset=(nspec+5);
    int s4d_offset=1;
    int z3d_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1]);
    int y3d_offset=(n[1]+2*ng[1]);
    int x3d_offset=1;
    
    foreach(i=(ns[0]+ng[0])...(ne[0]+ng[0]),j=(ns[1]+ng[1])...(ne[1]+ng[1]),k=(ns[2]+ng[2])...(ne[2]+ng[2])){
        int fijk=(k-ng[2])*n[1]*n[0]*(nspec+5)+(j-ng[1])*n[0]*(nspec+5)+(i-ng[0])*(nspec+5);
        
        int ip1jk=k*z4d_offset+j*y4d_offset+(i+1)*x4d_offset;
        int ip2jk=k*z4d_offset+j*y4d_offset+(i+2)*x4d_offset;
        int ip3jk=k*z4d_offset+j*y4d_offset+(i+3)*x4d_offset;
        int ip4jk=k*z4d_offset+j*y4d_offset+(i+4)*x4d_offset;

        int im1jk=k*z4d_offset+j*y4d_offset+(i-1)*x4d_offset;
        int im2jk=k*z4d_offset+j*y4d_offset+(i-2)*x4d_offset;
        int im3jk=k*z4d_offset+j*y4d_offset+(i-3)*x4d_offset;
        int im4jk=k*z4d_offset+j*y4d_offset+(i-4)*x4d_offset;

        int presip1jk=k*z3d_offset+j*y3d_offset+(i+1)*x3d_offset;
        int presip2jk=k*z3d_offset+j*y3d_offset+(i+2)*x3d_offset;
        int presip3jk=k*z3d_offset+j*y3d_offset+(i+3)*x3d_offset;
        int presip4jk=k*z3d_offset+j*y3d_offset+(i+4)*x3d_offset;

        int presim1jk=k*z3d_offset+j*y3d_offset+(i-1)*x3d_offset;
        int presim2jk=k*z3d_offset+j*y3d_offset+(i-2)*x3d_offset;
        int presim3jk=k*z3d_offset+j*y3d_offset+(i-3)*x3d_offset;
        int presim4jk=k*z3d_offset+j*y3d_offset+(i-4)*x3d_offset;


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
        (ALPHA*(cons[ip1jk+I_MX]*unp1-cons[im1jk+I_MX]*unm1+(pres[presip1jk]-pres[presim1jk]))
        +BETA *(cons[ip2jk+I_MX]*unp2-cons[im2jk+I_MX]*unm2+(pres[presip2jk]-pres[presim2jk]))
        +GAMMA*(cons[ip3jk+I_MX]*unp3-cons[im3jk+I_MX]*unm3+(pres[presip3jk]-pres[presim3jk]))
        +DELTA*(cons[ip4jk+I_MX]*unp4-cons[im4jk+I_MX]*unm4+(pres[presip4jk]-pres[presim4jk])))/dx[0];

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
        (ALPHA*(cons[ip1jk+I_ENE]*unp1-cons[im1jk+I_ENE]*unm1+(pres[presip1jk]*unp1-pres[presim1jk]*unm1))
        +BETA *(cons[ip2jk+I_ENE]*unp2-cons[im2jk+I_ENE]*unm2+(pres[presip2jk]*unp2-pres[presim2jk]*unm2))
        +GAMMA*(cons[ip3jk+I_ENE]*unp3-cons[im3jk+I_ENE]*unm3+(pres[presip3jk]*unp3-pres[presim3jk]*unm3))
        +DELTA*(cons[ip4jk+I_ENE]*unp4-cons[im4jk+I_ENE]*unm4+(pres[presip4jk]*unp4-pres[presim4jk]*unm4)))/dx[0];
        
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

        int presijp1k=k*z3d_offset+(j+1)*y3d_offset+i*x3d_offset;
        int presijp2k=k*z3d_offset+(j+2)*y3d_offset+i*x3d_offset;
        int presijp3k=k*z3d_offset+(j+3)*y3d_offset+i*x3d_offset;
        int presijp4k=k*z3d_offset+(j+4)*y3d_offset+i*x3d_offset;
        
        int presijm1k=k*z3d_offset+(j-1)*y3d_offset+i*x3d_offset;
        int presijm2k=k*z3d_offset+(j-2)*y3d_offset+i*x3d_offset;
        int presijm3k=k*z3d_offset+(j-3)*y3d_offset+i*x3d_offset;
        int presijm4k=k*z3d_offset+(j-4)*y3d_offset+i*x3d_offset;

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
        (ALPHA*(cons[ijp1k+I_MY]*unp1-cons[ijm1k+I_MY]*unm1+(pres[presijp1k]-pres[presijm1k]))
        +BETA *(cons[ijp2k+I_MY]*unp2-cons[ijm2k+I_MY]*unm2+(pres[presijp2k]-pres[presijm2k]))
        +GAMMA*(cons[ijp3k+I_MY]*unp3-cons[ijm3k+I_MY]*unm3+(pres[presijp3k]-pres[presijm3k]))
        +DELTA*(cons[ijp4k+I_MY]*unp4-cons[ijm4k+I_MY]*unm4+(pres[presijp4k]-pres[presijm4k])))/dx[1];

        flux[fijk+I_MZ]-=
        (ALPHA*(cons[ijp1k+I_MZ]*unp1-cons[ijm1k+I_MZ]*unm1)
        +BETA *(cons[ijp2k+I_MZ]*unp2-cons[ijm2k+I_MZ]*unm2)
        +GAMMA*(cons[ijp3k+I_MZ]*unp3-cons[ijm3k+I_MZ]*unm3)
        +DELTA*(cons[ijp4k+I_MZ]*unp4-cons[ijm4k+I_MZ]*unm4))/dx[1];

        flux[fijk+I_ENE]-=
        (ALPHA*(cons[ijp1k+I_ENE]*unp1-cons[ijm1k+I_ENE]*unm1+(pres[presijp1k]*unp1-pres[presijm1k]*unm1))
        +BETA *(cons[ijp2k+I_ENE]*unp2-cons[ijm2k+I_ENE]*unm2+(pres[presijp2k]*unp2-pres[presijm2k]*unm2))
        +GAMMA*(cons[ijp3k+I_ENE]*unp3-cons[ijm3k+I_ENE]*unm3+(pres[presijp3k]*unp3-pres[presijm3k]*unm3))
        +DELTA*(cons[ijp4k+I_ENE]*unp4-cons[ijm4k+I_ENE]*unm4+(pres[presijp4k]*unp4-pres[presijm4k]*unm4)))/dx[1];

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

        int presijkp1=(k+1)*z3d_offset+j*y3d_offset+i*x3d_offset;
        int presijkp2=(k+2)*z3d_offset+j*y3d_offset+i*x3d_offset;
        int presijkp3=(k+3)*z3d_offset+j*y3d_offset+i*x3d_offset;
        int presijkp4=(k+4)*z3d_offset+j*y3d_offset+i*x3d_offset;
        
        int presijkm1=(k-1)*z3d_offset+j*y3d_offset+i*x3d_offset;
        int presijkm2=(k-2)*z3d_offset+j*y3d_offset+i*x3d_offset;
        int presijkm3=(k-3)*z3d_offset+j*y3d_offset+i*x3d_offset;
        int presijkm4=(k-4)*z3d_offset+j*y3d_offset+i*x3d_offset;
        
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
        (ALPHA*(cons[ijkp1+I_MZ]*unp1-cons[ijkm1+I_MZ]*unm1+(pres[presijkp1]-pres[presijkm1]))
        +BETA *(cons[ijkp2+I_MZ]*unp2-cons[ijkm2+I_MZ]*unm2+(pres[presijkp2]-pres[presijkm2]))
        +GAMMA*(cons[ijkp3+I_MZ]*unp3-cons[ijkm3+I_MZ]*unm3+(pres[presijkp3]-pres[presijkm3]))
        +DELTA*(cons[ijkp4+I_MZ]*unp4-cons[ijkm4+I_MZ]*unm4+(pres[presijkp4]-pres[presijkm4])))/dx[2];

        flux[fijk+I_ENE]-=
        (ALPHA*(cons[ijkp1+I_ENE]*unp1-cons[ijkm1+I_ENE]*unm1+(pres[presijkp1]*unp1-pres[presijkm1]*unm1))
        +BETA *(cons[ijkp2+I_ENE]*unp2-cons[ijkm2+I_ENE]*unm2+(pres[presijkp2]*unp2-pres[presijkm2]*unm2))
        +GAMMA*(cons[ijkp3+I_ENE]*unp3-cons[ijkm3+I_ENE]*unm3+(pres[presijkp3]*unp3-pres[presijkm3]*unm3))
        +DELTA*(cons[ijkp4+I_ENE]*unp4-cons[ijkm4+I_ENE]*unm4+(pres[presijkp4]*unp4-pres[presijkm4]*unm4)))/dx[2];

        for(int nsp=I_SP;nsp<nspec+5;nsp++){
           flux[fijk+nsp]-=
           (ALPHA*(cons[ijkp1+nsp]*unp1-cons[ijkm1+nsp]*unm1)
           +BETA *(cons[ijkp2+nsp]*unp2-cons[ijkm2+nsp]*unm2)
           +GAMMA*(cons[ijkp3+nsp]*unp3-cons[ijkm3+nsp]*unm3)
           +DELTA*(cons[ijkp4+nsp]*unp4-cons[ijkm4+nsp]*unm4))/dx[2];
        }
    }
}
