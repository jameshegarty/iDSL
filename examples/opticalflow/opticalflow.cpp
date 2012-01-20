//
//  opticalflow
//
//

#include <stdio.h>
#include "../util.h"


// See pg 189 of Horn & Schunk, "Determining Optical Flow"
struct PartialDerivs {
  float Ex;
  float Ey;
  float Et;
};


static void optflow_dummyimpl(const unsigned char *data1, const unsigned char *data2, unsigned char **dataout,
                                int width, int height, int channels) {
    
    *dataout = new unsigned char[width*height*channels];
    
    for(int c=0; c<channels; c++){
        
        for(int i=0; i<width*height; i++){
	  (*dataout)[i*channels+c] = data1[i*channels + c];// + data2[i*channels + c];
        }
    }
}



static void estimatePartial(const unsigned char *img_t1, 
			    const unsigned char *img_t2,
			    struct PartialDerivs *partials,
			    int width, int height,
			    int i, int j, int chan) {

  float E_i_j_k = sampleNearest(width, height, i, j, img_t1);
  float E_i1_j_k = sampleNearest(width, height, i+1, j, img_t1);
  float E_i1_j1_k = sampleNearest(width, height, i+1, j+1, img_t1);
  float E_i_j1_k = sampleNearest(width, height, i, j+1, img_t1);

  float E_i_j_k1 = sampleNearest(width, height, i, j, img_t2);
  float E_i1_j_k1 = sampleNearest(width, height, i+1, j, img_t2);
  float E_i1_j1_k1 = sampleNearest(width, height, i+1, j+1, img_t2);
  float E_i_j1_k1 = sampleNearest(width, height, i, j+1, img_t2);


  // These equations are at the bottom of pg 189:
  // "Each of the estimates is the average of four first differences
  // taken over adjacent measurements in the cube."
  partials->Ex = 0.25 * (E_i_j1_k - E_i_j_k
			 + E_i1_j1_k - E_i1_j_k
			 + E_i_j1_k1 - E_i_j_k1
			 + E_i1_j1_k1 - E_i1_j_k1);

  partials->Ey = 0.25 * (E_i1_j_k - E_i_j_k
			 + E_i1_j1_k - E_i_j1_k
			 + E_i1_j_k1 - E_i_j_k1
			 + E_i1_j1_k1 - E_i_j1_k1);

  partials->Et = 0.25 * (E_i_j_k1 - E_i_j_k
			 + E_i1_j_k1 - E_i1_j_k
			 + E_i_j1_k1 - E_i_j1_k
			 + E_i1_j1_k1 - E_i1_j1_k);
  
}
			     


int main (int argc, const char * argv[])
{

    if(argc!=4){
        printf("Usage: opticalflow image1.bmp image2.bmp out.flo\n");
        return 1;
    }
    
    int width1, height1, channels1;
    int width2, height2, channels2;

    unsigned char *data1, *data2, *dataout;
    
    bool res1 = loadImage(argv[1], &width1, &height1, &channels1, &data1);
    bool res2 = loadImage(argv[2], &width2, &height2, &channels2, &data2);

    if (!res1 || !res2) {
        
        printf("Error loading images. Exiting...\n");
        return -1;
    }
    
    if (width1 != width2 || height1 != height2 || channels1 != channels2) {
        printf("Image dimensions don't match. Exiting...\n");
        
        delete [] data1;
        delete [] data2;
        return -1;
    }
    
    optflow_dummyimpl(data1, data2, &dataout, width1, height1, channels1);

    // save the image
    saveImage(argv[3], width1, height1, channels1, dataout);

    PartialDerivs p;
    estimatePartial(data1, data2, &p, width1, height1, 3, 4, channels1);


    delete [] data1;
    delete [] data2;
    delete [] dataout;
    
    return 0;
}

