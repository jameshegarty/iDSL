//
//  main.cpp
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#include <iostream>
#include "Pipeline.h"
#include "util.h"
#include "pipelineConfig.h"


// Using this to name the output frames.
// Although not threadsafe, I'm only modifying this from one thread
// (the final stage, which saves images to disk) so it's fine.
static int counter = 0;


// Unary color transformation. Just a simple experiment.
void *colordarkenstage (void *args);
void *colordarkenstage (void *args) {
    Image *img = (Image *)args;

    int w = img->width;
    int h = img->height;
    
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            
            unsigned char red = readPixel(w, h, img->channels, j, i, RED, img->data);
            unsigned char green = readPixel(w, h, img->channels, j, i, GREEN, img->data);
            unsigned char blue = readPixel(w, h, img->channels, j, i, BLUE, img->data);
            
            // FOR TESTING ONLY
            //            int wcolor = 255 * (float)j / (float)(w);
            //            int hcolor = 255 * (float)i / (float)(img->height);
            
            writePixel(w, h, img->channels, j, i, RED, img->data, 0.5 * red);
            writePixel(w, h, img->channels, j, i, GREEN, img->data, 0.5 * green);
            writePixel(w, h, img->channels, j, i, BLUE, img->data, 0.5 * blue);
        }
    }
    
    return img;
}


// Saves output frames to disk. Not a real pipeline stage, but
// useful for the simulation.
void *finalstage (void *args);
void *finalstage (void *args) {
    
    // Add a padding zero for small numbers
    const char *zero = counter < 10 ? "0" : "";

    char *framename = new char[strlen(OUTPUT_DIRECTORY) + 4 + strlen(zero) + 1];
    sprintf(framename, OUTPUT_DIRECTORY, zero, counter++);
    
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
    
    
    // Give it some code that actually does stuff:
    p.setStageCode(2, &colordarkenstage);
    p.setStageCode(4, &finalstage);
    
    // Hook up the sensor, launch the simulation!
    p.connectSensor(&s);
    p.powerOn();
    
    
    // just to prevent program from exiting immediately
    // (because separate threads are launched for the stages)
    sleep(4);
    
    std::cout << "...shutting down!\n";
    
    return 0;
}
