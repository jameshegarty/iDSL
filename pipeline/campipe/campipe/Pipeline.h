//
//  Pipeline.h
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#ifndef campipe_Pipeline_h
#define campipe_Pipeline_h

#include <pthread.h>
#include "CQueue.h"
#include "Sensor.h"
#include "Image.h"


/**
 * This is an example of a camera algorithm passed to a particular pipeline stage. 
 * (Obviously, this is fake and does nothing)
 */

static void *fake_camera_algo(void *arg);

void *fake_camera_algo(void *arg) {
    
    Image *fakeargs = (Image *)arg;
    
    
   // printf("stage %s. Now  processing %s\n", fakeargs->stageName, fakeargs->frameName);
    return fakeargs;
}


enum EStageType {EST_Fixed, EST_Configurable, EST_Programmable};

struct PStage {
    CQueue *inQueue;
    CQueue *outQueue;
    void *(*fn)(void *);
    const char *stageID;
};

/**
 * PipelineStage
 * =============
 * Represents a single stage in a configurable pipeline.
 * 
 */
class PipelineStage {
    
public:
    
    /**
     * 
     */
    PipelineStage(EStageType stageType, const char *stageID);
    
    
    /**
     * 
     */
    ~PipelineStage() {}
    
    
    void setQueues(CQueue *inQueue, CQueue *outQueue) {
        m_inQueue = inQueue; 
        m_outQueue = outQueue;
    }
    
    bool powerOn();

    void setCode(void *(*fn)(void *)) { m_fn = fn; }

    
private:
    
    /**
     * Static callback invoked by new thread. 
     * (Contains infinite loop)
     */
    static void *runStage (void *vstage);
    
    
    void *(*m_fn)(void *);
    pthread_t m_tid;
    const char *m_stageID;
    
    CQueue *m_inQueue, *m_outQueue;
    
};




class Pipeline {
    
public:

    Pipeline(size_t nStages, EStageType stageTypes[]);
    ~Pipeline();
    
    void connectSensor(Sensor *sensor);

    
    /**
     * Powers on each pipeline stage.
     */
    bool powerOn();
    
    void setStageCode(int stageNum, void *(*fn)(void *)) {
        m_stages[stageNum]->setCode(fn);
    }
    
    
    
private:
    size_t m_nStages;
    PipelineStage **m_stages;
    CQueue *m_queues;
    
    Sensor *m_sensor;
};


#endif
