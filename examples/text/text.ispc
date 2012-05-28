export void dist(
  const uniform unsigned int8 in[],       
  uniform unsigned int8 upVec[], 
  uniform unsigned int8 downVec[], 
  uniform unsigned int8 leftVec[], 
  uniform unsigned int8 rightVec[],
  const uniform int width,
  const uniform int height){

  // compute up vec (vector pointing to nearest thing up)
  foreach(x = 0 ... width){upVec[x]=0;}
  foreach(y= 1 ... height,x = 0 ... width){ if(in[x+y*width]){ upVec[x+y*width]=0; }else{ upVec[x+y*width]=upVec[x+(y-1)*width]+1; } }

  // downVecs
  foreach(x = 0 ... width){downVec[x+(height-1)*width]=0;}
  foreach(ty= 1 ... height,x = 0 ... width){ int32 y=height-ty-1;if(in[x+y*width]){ downVec[x+y*width]=0; }else{ downVec[x+y*width]=downVec[x+(y+1)*width]+1; } }

  // leftVec (vec pointing to nearest thing left)
  foreach(y = 0 ... height){leftVec[y*width]=0;}
  foreach(x = 1 ... width, y= 0 ... height){ leftVec[x+y*width]=(in[x+y*width])?0:leftVec[(x-1)+y*width]+1; }

  // rightVec (vec pointing to nearest thing right)
  foreach(y = 0 ... height){rightVec[width-1+y*width]=0;}
  foreach(tx = 1 ... width, y= 0 ... height){ int32 x=width-tx-1;rightVec[x+y*width]=(in[x+y*width])?0:rightVec[(x+1)+y*width]+1; }

}

export void simple(
  const uniform unsigned int8 in[], 
  const uniform unsigned int8 upVec[], 
  const uniform unsigned int8 downVec[], 
  const uniform unsigned int8 leftVec[], 
  const uniform unsigned int8 rightVec[], 
  uniform unsigned int8 temp[],
  uniform unsigned int8 out[],
  const uniform int width, const uniform int height,
  const uniform int startY, const uniform int endY) {

  uniform int32 id = 1;
		   
  // rake along x
  foreach(x = 1 ... width,y= startY ... endY){
    if(in[x+y*width] != in[(x-1)+y*width]){
      if(in[x+y*width]){
        out[x+y*width]=atomic_add_global(&id,(varying int32)1);
      }else{
        out[x+y*width]=0;
      }
    }else{
      out[x+y*width] = out[(x-1)+y*width];
    }
  }

/*
  // rake along y
  foreach(y= startY ... endY, x = 1 ... width){
    if(in[x+y*width] != in[x+(y-1)*width]){
      if(in[x+y*width]){
        out[x+y*width]=atomic_add_global(&id,(varying int32)1);
      }else{
        out[x+y*width]=0;
      }
    }else{
      out[x+y*width] = out[x+(y-1)*width];
    }
  }
*/

  uniform int32 rename[256];
  //for(int i=0; i<256; i++){ rename[i]=i; }
  foreach(i = 0 ... 256){rename[i]=i;}

  // prop in y

  foreach(y= startY+1 ... endY, x = 1 ... width){
    temp[x+y*width]=out[x+y*width];
  }

  foreach(y= startY+1 ... endY){
    foreach(x = 1 ... width){
      int32 lab = temp[x+(y-1)*width];
      if(lab!=0){
        atomic_min_local(&(rename[temp[x+y*width]]),lab);
      }

      // now prop
      //unsigned int8 offL = -leftVec[x+(y-1)*width];
      //unsigned int8 offR = rightVec[x+(y-1)*width];
      
      temp[x+y*width]=rename[temp[x+y*width]];
    }

    //for(int i=0; i<256; i++){ rename[i]=i; }
    //foreach(i = 0 ... 256){rename[i]=i;}
  }


//  foreach(x = 0 ... width, y= 0 ... height){
//  foreach(x = 0 ... width){
//    vout[x]=1;
//  }

//    uniform int32 lowest = 255;

  foreach(y= 0 ... height,x = 0 ... width){
//    vout[x+(height-y-1)*width]=x;
  }

  foreach(y= 100 ... height,x = 0 ... width){
//    vout[x+y*width]=vout[x+(y-1)*width]+1;
//      vout[x+y*width]= atomic_max_local(&lowest,vout[x+(y-1)*width]);
  }
		   
/*    foreach (index = 0 ... count) {
        // Load the appropriate input value for this program instance.
        float v = vin[index];

        // Do an arbitrary little computation, but at least make the
        // computation dependent on the value being processed
        if (v < 3.)
            v = v * v;
        else
            v = sqrt(v);

        // And write the result to the output array.
        vout[index] = v;
    }*/
}
