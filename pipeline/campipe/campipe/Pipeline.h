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


// This function is a pipeline stage that does nothing.
// It is the default function if no custom function is provided.
// Should never be used in a real program, so it's here only
// for testing.
static void *empty_stage(void *arg);
void *empty_stage(void *arg) {
    return arg;
}


enum EStageType {EST_Fixed, EST_Configurable, EST_Programmable};




/**
 * PipelineStage
 * =============
 * Represents a single stage in a configurable pipeline.
 * 
 */
class PipelineStage {
    
public:
    
    PipelineStage(EStageType stageType, const char *stageID);
    ~PipelineStage() {}
    
    
    void setQueues(CQueue *inQueue, CQueue *outQueue) {
        m_inQueue = inQueue; 
        m_outQueue = outQueue;
    }
    
    bool powerOn();

    void setCode(void *(*fn)(void *)) { m_fn = fn; }
    void setNFrames(int nFrames) { m_nFrames = nFrames; }
    
private:
    
    /**
     * Static callback invoked by new thread. 
     * (Contains infinite loop)
     */
    static void *runStage (void *vstage);
    
    
    void *(*m_fn)(void *);
    pthread_t m_tid;
    const char *m_stageID;
    int m_nFrames;
    
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
    
    void setStageCode(int stageNum, void *(*fn)(void *), int nFrames = 1) {
        m_stages[stageNum]->setCode(fn);
        m_stages[stageNum]->setNFrames(nFrames);
    }
    
    
    
private:
    size_t m_nStages;
    PipelineStage **m_stages;
    CQueue *m_queues;
    
    Sensor *m_sensor;
};


#endif
