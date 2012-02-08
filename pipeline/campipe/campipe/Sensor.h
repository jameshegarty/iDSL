//
//  Sensor.h
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#ifndef campipe_Sensor_h
#define campipe_Sensor_h

#include "CQueue.h"



class Sensor {
  
public:
    Sensor(size_t fps);
    ~Sensor() {}
    
    // Connects the sensor to a queue
    void setQueue(CQueue *q);
    
    // Callback that runs in a separate thread.
    // Simulates a sensor that produces "m_fps" frames every second.
    static void *produceFrames (void *args);

    // Launches thread that calls produceFrames
    bool powerOn();
    
    
private:
    pthread_t m_tid;
    size_t m_fps;
    CQueue *m_queue;
};


#endif
