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
    
    void setQueue(CQueue *q);
    
    static void *produceFrames (void *args);
    
    bool powerOn();
    
    
private:
    pthread_t m_tid;
    size_t m_fps;
    CQueue *m_queue;
};


#endif
