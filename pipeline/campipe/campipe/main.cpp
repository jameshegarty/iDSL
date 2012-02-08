//
//  main.cpp
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#include <iostream>
#include "Pipeline.h"
#include "stagecode.h"


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
    p.setStageCode(1, &sillymerge, 2);
    p.setStageCode(4, &finalstage);
    
    // Hook up the sensor, launch the simulation!
    p.connectSensor(&s);
    p.powerOn();
    
    
    // just to prevent program from exiting immediately
    // (because separate threads are launched for the stages)
    sleep(2);
    
    std::cout << "...shutting down!\n";
    
    return 0;
}
