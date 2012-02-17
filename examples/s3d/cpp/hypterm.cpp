#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "constants.h"

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
void hypterm(const int *n,const int *ng,const double *dx,const int nspec,double ****cons,double ***pres,double ****flux){
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
                std::cout << flux[i-ng[0]][j-ng[1]][k-ng[2]][I_RHO] << std::endl;
            }
        }
    }
}

int main(int argc, const char *argv[]){
    
    const int nspec=9;
    const double dx[3]={1e-3,1e-3,1e-3};
    
    const int n[3]={4,4,4};
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
    init(n,ng,dx,nspec,cons,pres);
    hypterm(n,ng,dx,nspec,cons,pres,flux);

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
        std::cout<<"component, fluxmag "<<ns<<" "<<std::setprecision(16)<<fluxmag[ns]<<std::endl;
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
}
