//
//  Sensor.cpp
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>


#include "Sensor.h"
#include "Image.h"
#include "util.h"

struct SensorThreadArgs {
    size_t fps;
    CQueue *queue;
};


Sensor::Sensor(size_t fps) : m_fps(fps) {}

void Sensor::setQueue(CQueue *q) { 
    m_queue = q; 
}


void *Sensor::produceFrames (void *args) {
    
    int frameCounter = 0;
    
    SensorThreadArgs *sensorargs = (SensorThreadArgs *)args;
    
    useconds_t wavelength = 1000000 / (int)sensorargs->fps;
    
    
    // TODO: LOTS OF SILLY MAGIC NUMS HERE
    char *framenums[14];
    for (int i = 0; i < 14; i++) {
        const char *dir = "/Users/phaedon/dogdance/frame%s%d.bmp";
        framenums[i] = new char[strlen(dir) + 4 + 3 + 1];
        const char *zero = i < 10 ? "0" : "";
        sprintf(framenums[i], dir, zero, i);
        printf("%s\n", framenums[i]);
    }
         
    
        
    int counter = 0;
    
    while (true) {
        
        usleep (wavelength);
        
        char *imageContents = new char[strlen("img frame ______")];
        sprintf(imageContents, "img frame %d", ++frameCounter);
        
        Image *fake = new Image;
        fake->stageName = "sensor";
        fake->frameName = imageContents;

        loadImage(framenums[counter], &fake->width, &fake->height, &fake->channels, &fake->data);
        
        ++counter;
        if (counter > 13) counter = 0;
        
        assert(fake != NULL);
        sensorargs->queue->push((void*)fake);
        
    }
    
}

bool Sensor::powerOn() {
    
    SensorThreadArgs *args = new SensorThreadArgs;
    args->fps = m_fps;
    args->queue = m_queue;
    
    pthread_create(&m_tid, NULL, &produceFrames, args);
    
    return true;
}

