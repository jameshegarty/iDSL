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

void *colordarkenstage (void *args);

void *colordarkenstage (void *args) {
    Image *img = (Image *)args;

    int w = img->width;
    int h = img->height;
    
    // bmps pad each row to be a multiple of 4 bytes
    int padding = 4 - (w * 3 % 4); // we always write 3 channels
    padding = padding == 4 ? 0 : padding;
    
    //printf("padding: %d\n", padding);
    //printf("width: %d   height: %d\n", w, h);
    
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
    
    
    // Give it some code that actually does stuff:
    p.setStageCode(2, &colordarkenstage);
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
