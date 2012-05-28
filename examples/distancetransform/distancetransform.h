template<class T>
void distanceTransform(unsigned char* in, T *out, unsigned char background, int width, int height){
  // pass 1
  for(int y=1; y<height; y++){
    for(int x=1; x<width; x++){
      if(in[x+y*width]==background){
	out[x+y*width]=0;
      }else{
	float a = pow(sqrt((float)out[(x-1)+y*width])+1.f,2);
	float b = pow(sqrt((float)out[x+(y-1)*width])+1.f,2);
	float c = pow(sqrt((float)out[(x-1)+(y-1)*width])+1.f,2);
	out[x+y*width]=std::min(std::min(a,b),c);
      }
    }
  }

  for(int y=height-2; y>=0; y--){
    for(int x=width-2; x>=0; x--){
      if(in[x+y*width]==background){
	out[x+y*width]=0;
      }else{
	float a = pow(sqrt((float)out[(x+1)+y*width])+1.f,2);
	float b = pow(sqrt((float)out[x+(y+1)*width])+1.f,2);
	float c = pow(sqrt((float)out[(x+1)+(y+1)*width])+1.f,2);
	float d = pow(sqrt((float)out[x+y*width])+1.f,2);
	out[x+y*width]=std::min(std::min(std::min(a,b),c),d);
      }
    }
  }

}
