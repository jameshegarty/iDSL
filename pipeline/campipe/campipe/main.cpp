//
//  main.cpp
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#include <iostream>
#include "Pipeline.h"
#include "util.h"

static int counter = 0;

void *finalstage (void *args);

void *finalstage (void *args) {
    
    // TODO: LOTS OF SILLY MAGIC NUMS HERE
    const char *dir = "/Users/phaedon/dogdance/output/frame%s%d.bmp";
    char *framename = new char[strlen(dir) + 4 + 3 + 1];
    const char *zero = counter < 10 ? "0" : "";
    sprintf(framename, dir, zero, counter++);
    //printf("%s\n", framename);
    
    Image *img = (Image *)args;
    
    saveImage(framename, img->width, img->height, img->channels, img->data);
    
    return NULL;
}


int main (int argc, const char * argv[])
{

    // This doesn't do anything yet. It's just a random
    // assortment of flags:
    EStageType stages[] =  {
        EST_Fixed, 
        EST_Fixed, 
        EST_Programmable, 
        EST_Configurable, 
        EST_Fixed};
    
    // Let's create a 12-fps sensor and a 5-stage imaging pipeline
    Sensor s(12);
    Pipeline p(5, stages);
    
    
    // Give it the "final-stage" code which simply saves to disk:
    p.setStageCode(4, &finalstage);
    
    // Hook up the sensor, launch the simulation!
    p.connectSensor(&s);
    p.powerOn();
    
    
    // just to prevent program from exiting immediately
    // (because separate threads are launched for the stages)
    sleep(2);
    
    std::cout << "Hello, World!\n";
    
    return 0;
}
