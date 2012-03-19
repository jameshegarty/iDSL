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

    int s_offset=(n[2]+2*ng[2])*(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int z_offset=(n[1]+2*ng[1])*(n[0]+2*ng[0]);
    int y_offset=(n[0]+2*ng[0]);
    int x_offset=1;
    
    foreach(k=(ns[2]+ng[2])...(ne[2]+ng[2]),j=(ns[1]+ng[1])...(ne[1]+ng[1]),i=(ns[0]+ng[0])...(ne[0]+ng[0])){
        int fijk=(k-ng[2])*n[1]*n[0]+(j-ng[1])*n[0]+(i-ng[0]);
        
        int ip1jk=k*z_offset+j*y_offset+(i+1);
        int ip2jk=k*z_offset+j*y_offset+(i+2);
        int ip3jk=k*z_offset+j*y_offset+(i+3);
        int ip4jk=k*z_offset+j*y_offset+(i+4);

        int im1jk=k*z_offset+j*y_offset+(i-1);
        int im2jk=k*z_offset+j*y_offset+(i-2);
        int im3jk=k*z_offset+j*y_offset+(i-3);
        int im4jk=k*z_offset+j*y_offset+(i-4);

        double unp1=cons[ip1jk+s_offset*I_MX]/cons[ip1jk+s_offset*I_RHO];
        double unp2=cons[ip2jk+s_offset*I_MX]/cons[ip2jk+s_offset*I_RHO];
        double unp3=cons[ip3jk+s_offset*I_MX]/cons[ip3jk+s_offset*I_RHO];
        double unp4=cons[ip4jk+s_offset*I_MX]/cons[ip4jk+s_offset*I_RHO];
                                                     
        double unm1=cons[im1jk+s_offset*I_MX]/cons[im1jk+s_offset*I_RHO];
        double unm2=cons[im2jk+s_offset*I_MX]/cons[im2jk+s_offset*I_RHO];
        double unm3=cons[im3jk+s_offset*I_MX]/cons[im3jk+s_offset*I_RHO];
        double unm4=cons[im4jk+s_offset*I_MX]/cons[im4jk+s_offset*I_RHO];

        flux[fijk+n[0]*n[1]*n[2]*I_RHO]-=
        (ALPHA*(cons[ip1jk+s_offset*I_MX]-cons[im1jk+s_offset*I_MX])
        +BETA *(cons[ip2jk+s_offset*I_MX]-cons[im2jk+s_offset*I_MX])
        +GAMMA*(cons[ip3jk+s_offset*I_MX]-cons[im3jk+s_offset*I_MX])
        +DELTA*(cons[ip4jk+s_offset*I_MX]-cons[im4jk+s_offset*I_MX]))/dx[0];
        
        flux[fijk+n[0]*n[1]*n[2]*I_MX]-=
        (ALPHA*(cons[ip1jk+s_offset*I_MX]*unp1-cons[im1jk+s_offset*I_MX]*unm1+(pres[ip1jk]-pres[im1jk]))
        +BETA *(cons[ip2jk+s_offset*I_MX]*unp2-cons[im2jk+s_offset*I_MX]*unm2+(pres[ip2jk]-pres[im2jk]))
        +GAMMA*(cons[ip3jk+s_offset*I_MX]*unp3-cons[im3jk+s_offset*I_MX]*unm3+(pres[ip3jk]-pres[im3jk]))
        +DELTA*(cons[ip4jk+s_offset*I_MX]*unp4-cons[im4jk+s_offset*I_MX]*unm4+(pres[ip4jk]-pres[im4jk])))/dx[0];

        flux[fijk+n[2]*n[1]*n[0]*I_MY]-=
        (ALPHA*(cons[ip1jk+s_offset*I_MY]*unp1-cons[im1jk+s_offset*I_MY]*unm1)
        +BETA *(cons[ip2jk+s_offset*I_MY]*unp2-cons[im2jk+s_offset*I_MY]*unm2)
        +GAMMA*(cons[ip3jk+s_offset*I_MY]*unp3-cons[im3jk+s_offset*I_MY]*unm3)
        +DELTA*(cons[ip4jk+s_offset*I_MY]*unp4-cons[im4jk+s_offset*I_MY]*unm4))/dx[0];

        flux[fijk+n[2]*n[1]*n[0]*I_MZ]-=
        (ALPHA*(cons[ip1jk+s_offset*I_MZ]*unp1-cons[im1jk+s_offset*I_MZ]*unm1)
        +BETA *(cons[ip2jk+s_offset*I_MZ]*unp2-cons[im2jk+s_offset*I_MZ]*unm2)
        +GAMMA*(cons[ip3jk+s_offset*I_MZ]*unp3-cons[im3jk+s_offset*I_MZ]*unm3)
        +DELTA*(cons[ip4jk+s_offset*I_MZ]*unp4-cons[im4jk+s_offset*I_MZ]*unm4))/dx[0];

        flux[fijk+n[2]*n[1]*n[0]*I_ENE]-=
        (ALPHA*(cons[ip1jk+s_offset*I_ENE]*unp1-cons[im1jk+s_offset*I_ENE]*unm1+(pres[ip1jk]*unp1-pres[im1jk]*unm1))
        +BETA *(cons[ip2jk+s_offset*I_ENE]*unp2-cons[im2jk+s_offset*I_ENE]*unm2+(pres[ip2jk]*unp2-pres[im2jk]*unm2))
        +GAMMA*(cons[ip3jk+s_offset*I_ENE]*unp3-cons[im3jk+s_offset*I_ENE]*unm3+(pres[ip3jk]*unp3-pres[im3jk]*unm3))
        +DELTA*(cons[ip4jk+s_offset*I_ENE]*unp4-cons[im4jk+s_offset*I_ENE]*unm4+(pres[ip4jk]*unp4-pres[im4jk]*unm4)))/dx[0];
        
        for(int nsp=I_SP;nsp<nspec+5;nsp++){
            flux[fijk+n[2]*n[1]*n[0]*nsp]-=
            (ALPHA*(cons[ip1jk+s_offset*nsp]*unp1-cons[im1jk+s_offset*nsp]*unm1)
            +BETA *(cons[ip2jk+s_offset*nsp]*unp2-cons[im2jk+s_offset*nsp]*unm2)
            +GAMMA*(cons[ip3jk+s_offset*nsp]*unp3-cons[im3jk+s_offset*nsp]*unm3)
            +DELTA*(cons[ip4jk+s_offset*nsp]*unp4-cons[im4jk+s_offset*nsp]*unm4))/dx[0];
        }

        int ijp1k=k*z_offset+(j+1)*y_offset+i;
        int ijp2k=k*z_offset+(j+2)*y_offset+i;
        int ijp3k=k*z_offset+(j+3)*y_offset+i;
        int ijp4k=k*z_offset+(j+4)*y_offset+i;
        
        int ijm1k=k*z_offset+(j-1)*y_offset+i;
        int ijm2k=k*z_offset+(j-2)*y_offset+i;
        int ijm3k=k*z_offset+(j-3)*y_offset+i;
        int ijm4k=k*z_offset+(j-4)*y_offset+i;

        unp1 = cons[ijp1k+s_offset*I_MY]/cons[ijp1k+s_offset*I_RHO];
        unp2 = cons[ijp2k+s_offset*I_MY]/cons[ijp2k+s_offset*I_RHO];
        unp3 = cons[ijp3k+s_offset*I_MY]/cons[ijp3k+s_offset*I_RHO];
        unp4 = cons[ijp4k+s_offset*I_MY]/cons[ijp4k+s_offset*I_RHO];
    
        unm1 = cons[ijm1k+s_offset*I_MY]/cons[ijm1k+s_offset*I_RHO];
        unm2 = cons[ijm2k+s_offset*I_MY]/cons[ijm2k+s_offset*I_RHO];
        unm3 = cons[ijm3k+s_offset*I_MY]/cons[ijm3k+s_offset*I_RHO];
        unm4 = cons[ijm4k+s_offset*I_MY]/cons[ijm4k+s_offset*I_RHO];

        flux[fijk+n[2]*n[1]*n[0]*I_RHO]-=
        (ALPHA*(cons[ijp1k+s_offset*I_MY]-cons[ijm1k+s_offset*I_MY])
        +BETA *(cons[ijp2k+s_offset*I_MY]-cons[ijm2k+s_offset*I_MY])
        +GAMMA*(cons[ijp3k+s_offset*I_MY]-cons[ijm3k+s_offset*I_MY])
        +DELTA*(cons[ijp4k+s_offset*I_MY]-cons[ijm4k+s_offset*I_MY]))/dx[1];

        flux[fijk+n[2]*n[1]*n[0]*I_MX]-=
        (ALPHA*(cons[ijp1k+s_offset*I_MX]*unp1-cons[ijm1k+s_offset*I_MX]*unm1)
        +BETA *(cons[ijp2k+s_offset*I_MX]*unp2-cons[ijm2k+s_offset*I_MX]*unm2)
        +GAMMA*(cons[ijp3k+s_offset*I_MX]*unp3-cons[ijm3k+s_offset*I_MX]*unm3)
        +DELTA*(cons[ijp4k+s_offset*I_MX]*unp4-cons[ijm4k+s_offset*I_MX]*unm4))/dx[1];

        flux[fijk+n[2]*n[1]*n[0]*I_MY]-=
        (ALPHA*(cons[ijp1k+s_offset*I_MY]*unp1-cons[ijm1k+s_offset*I_MY]*unm1+(pres[ijp1k]-pres[ijm1k]))
        +BETA *(cons[ijp2k+s_offset*I_MY]*unp2-cons[ijm2k+s_offset*I_MY]*unm2+(pres[ijp2k]-pres[ijm2k]))
        +GAMMA*(cons[ijp3k+s_offset*I_MY]*unp3-cons[ijm3k+s_offset*I_MY]*unm3+(pres[ijp3k]-pres[ijm3k]))
        +DELTA*(cons[ijp4k+s_offset*I_MY]*unp4-cons[ijm4k+s_offset*I_MY]*unm4+(pres[ijp4k]-pres[ijm4k])))/dx[1];

        flux[fijk+n[2]*n[1]*n[0]*I_MZ]-=
        (ALPHA*(cons[ijp1k+s_offset*I_MZ]*unp1-cons[ijm1k+s_offset*I_MZ]*unm1)
        +BETA *(cons[ijp2k+s_offset*I_MZ]*unp2-cons[ijm2k+s_offset*I_MZ]*unm2)
        +GAMMA*(cons[ijp3k+s_offset*I_MZ]*unp3-cons[ijm3k+s_offset*I_MZ]*unm3)
        +DELTA*(cons[ijp4k+s_offset*I_MZ]*unp4-cons[ijm4k+s_offset*I_MZ]*unm4))/dx[1];

        flux[fijk+n[2]*n[1]*n[0]*I_ENE]-=
        (ALPHA*(cons[ijp1k+s_offset*I_ENE]*unp1-cons[ijm1k+s_offset*I_ENE]*unm1+(pres[ijp1k]*unp1-pres[ijm1k]*unm1))
        +BETA *(cons[ijp2k+s_offset*I_ENE]*unp2-cons[ijm2k+s_offset*I_ENE]*unm2+(pres[ijp2k]*unp2-pres[ijm2k]*unm2))
        +GAMMA*(cons[ijp3k+s_offset*I_ENE]*unp3-cons[ijm3k+s_offset*I_ENE]*unm3+(pres[ijp3k]*unp3-pres[ijm3k]*unm3))
        +DELTA*(cons[ijp4k+s_offset*I_ENE]*unp4-cons[ijm4k+s_offset*I_ENE]*unm4+(pres[ijp4k]*unp4-pres[ijm4k]*unm4)))/dx[1];

        for(int nsp=I_SP;nsp<nspec+5;nsp++){
           flux[fijk+n[2]*n[1]*n[0]*nsp]-=
           (ALPHA*(cons[ijp1k+s_offset*nsp]*unp1-cons[ijm1k+s_offset*nsp]*unm1)
           +BETA *(cons[ijp2k+s_offset*nsp]*unp2-cons[ijm2k+s_offset*nsp]*unm2)
           +GAMMA*(cons[ijp3k+s_offset*nsp]*unp3-cons[ijm3k+s_offset*nsp]*unm3)
           +DELTA*(cons[ijp4k+s_offset*nsp]*unp4-cons[ijm4k+s_offset*nsp]*unm4))/dx[1];
        }

        int ijkp1=(k+1)*z_offset+j*y_offset+i*x_offset;
        int ijkp2=(k+2)*z_offset+j*y_offset+i*x_offset;
        int ijkp3=(k+3)*z_offset+j*y_offset+i*x_offset;
        int ijkp4=(k+4)*z_offset+j*y_offset+i*x_offset;
                       
        int ijkm1=(k-1)*z_offset+j*y_offset+i*x_offset;
        int ijkm2=(k-2)*z_offset+j*y_offset+i*x_offset;
        int ijkm3=(k-3)*z_offset+j*y_offset+i*x_offset;
        int ijkm4=(k-4)*z_offset+j*y_offset+i*x_offset;
        
        unp1 = cons[ijkp1+s_offset*I_MZ]/cons[ijkp1+s_offset*I_RHO];
        unp2 = cons[ijkp2+s_offset*I_MZ]/cons[ijkp2+s_offset*I_RHO];
        unp3 = cons[ijkp3+s_offset*I_MZ]/cons[ijkp3+s_offset*I_RHO];
        unp4 = cons[ijkp4+s_offset*I_MZ]/cons[ijkp4+s_offset*I_RHO];

        unm1 = cons[ijkm1+s_offset*I_MZ]/cons[ijkm1+s_offset*I_RHO];
        unm2 = cons[ijkm2+s_offset*I_MZ]/cons[ijkm2+s_offset*I_RHO];
        unm3 = cons[ijkm3+s_offset*I_MZ]/cons[ijkm3+s_offset*I_RHO];
        unm4 = cons[ijkm4+s_offset*I_MZ]/cons[ijkm4+s_offset*I_RHO];

        flux[fijk+n[2]*n[1]*n[0]*I_RHO]-=
        (ALPHA*(cons[ijkp1+s_offset*I_MZ]-cons[ijkm1+s_offset*I_MZ])
        +BETA *(cons[ijkp2+s_offset*I_MZ]-cons[ijkm2+s_offset*I_MZ])
        +GAMMA*(cons[ijkp3+s_offset*I_MZ]-cons[ijkm3+s_offset*I_MZ])
        +DELTA*(cons[ijkp4+s_offset*I_MZ]-cons[ijkm4+s_offset*I_MZ]))/dx[2];

        flux[fijk+n[2]*n[1]*n[0]*I_MX]-=
        (ALPHA*(cons[ijkp1+s_offset*I_MX]*unp1-cons[ijkm1+s_offset*I_MX]*unm1)
        +BETA *(cons[ijkp2+s_offset*I_MX]*unp2-cons[ijkm2+s_offset*I_MX]*unm2)
        +GAMMA*(cons[ijkp3+s_offset*I_MX]*unp3-cons[ijkm3+s_offset*I_MX]*unm3)
        +DELTA*(cons[ijkp4+s_offset*I_MX]*unp4-cons[ijkm4+s_offset*I_MX]*unm4))/dx[2];

        flux[fijk+n[2]*n[1]*n[0]*I_MY]-=
        (ALPHA*(cons[ijkp1+s_offset*I_MY]*unp1-cons[ijkm1+s_offset*I_MY]*unm1)
        +BETA *(cons[ijkp2+s_offset*I_MY]*unp2-cons[ijkm2+s_offset*I_MY]*unm2)
        +GAMMA*(cons[ijkp3+s_offset*I_MY]*unp3-cons[ijkm3+s_offset*I_MY]*unm3)
        +DELTA*(cons[ijkp4+s_offset*I_MY]*unp4-cons[ijkm4+s_offset*I_MY]*unm4))/dx[2];

        flux[fijk+n[2]*n[1]*n[0]*I_MZ]-=
        (ALPHA*(cons[ijkp1+s_offset*I_MZ]*unp1-cons[ijkm1+s_offset*I_MZ]*unm1+(pres[ijkp1]-pres[ijkm1]))
        +BETA *(cons[ijkp2+s_offset*I_MZ]*unp2-cons[ijkm2+s_offset*I_MZ]*unm2+(pres[ijkp2]-pres[ijkm2]))
        +GAMMA*(cons[ijkp3+s_offset*I_MZ]*unp3-cons[ijkm3+s_offset*I_MZ]*unm3+(pres[ijkp3]-pres[ijkm3]))
        +DELTA*(cons[ijkp4+s_offset*I_MZ]*unp4-cons[ijkm4+s_offset*I_MZ]*unm4+(pres[ijkp4]-pres[ijkm4])))/dx[2];

        flux[fijk+n[2]*n[1]*n[0]*I_ENE]-=
        (ALPHA*(cons[ijkp1+s_offset*I_ENE]*unp1-cons[ijkm1+s_offset*I_ENE]*unm1+(pres[ijkp1]*unp1-pres[ijkm1]*unm1))
        +BETA *(cons[ijkp2+s_offset*I_ENE]*unp2-cons[ijkm2+s_offset*I_ENE]*unm2+(pres[ijkp2]*unp2-pres[ijkm2]*unm2))
        +GAMMA*(cons[ijkp3+s_offset*I_ENE]*unp3-cons[ijkm3+s_offset*I_ENE]*unm3+(pres[ijkp3]*unp3-pres[ijkm3]*unm3))
        +DELTA*(cons[ijkp4+s_offset*I_ENE]*unp4-cons[ijkm4+s_offset*I_ENE]*unm4+(pres[ijkp4]*unp4-pres[ijkm4]*unm4)))/dx[2];

        for(int nsp=I_SP;nsp<nspec+5;nsp++){
           flux[fijk+n[2]*n[1]*n[0]*nsp]-=
           (ALPHA*(cons[ijkp1+s_offset*nsp]*unp1-cons[ijkm1+s_offset*nsp]*unm1)
           +BETA *(cons[ijkp2+s_offset*nsp]*unp2-cons[ijkm2+s_offset*nsp]*unm2)
           +GAMMA*(cons[ijkp3+s_offset*nsp]*unp3-cons[ijkm3+s_offset*nsp]*unm3)
           +DELTA*(cons[ijkp4+s_offset*nsp]*unp4-cons[ijkm4+s_offset*nsp]*unm4))/dx[2];
        }
    }
}
