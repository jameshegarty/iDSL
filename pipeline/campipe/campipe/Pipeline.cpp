//
//  Pipeline.cpp
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#include <iostream>

#include "Pipeline.h"




// This struct is used to pass arguments to the runStage callback.
struct PStageThreadArgs {
    CQueue *inQueue;
    CQueue *outQueue;
    void *(*fn)(void *);
    const char *stageID;
    
    // Allows simultaneous processing of multiple images
    // (i.e. for image alignment)
    int numFrames;
};



/*******************
 * PIPELINESTAGE
 *******************/


PipelineStage::PipelineStage(EStageType stageType, const char *stageID)
: m_stageID(stageID) {
    
    printf("Initializing stage: %s\n", m_stageID);
    m_fn = &empty_stage;
    m_nFrames = 1;
}



bool PipelineStage::powerOn() { 
    
    PStageThreadArgs *stage = new PStageThreadArgs;
    stage->inQueue = m_inQueue;
    stage->outQueue = m_outQueue;
    stage->fn = m_fn;
    stage->stageID = m_stageID;
    stage->numFrames = m_nFrames;
    
    pthread_create(&m_tid, NULL, &runStage, (void *)stage);
    return true;
}


void *PipelineStage::runStage (void *vstage) {
    
    // Cast to the struct of arguments
    PStageThreadArgs *stage = (PStageThreadArgs *)vstage;
    
    while (true) {
        
        Image *frames[stage->numFrames];
        
        for (int i = 0; i < stage->numFrames; i++) {
            
            // Grab the appropriate number of frames off the queue
            // and place them into an array:
            
            frames[i] = (Image *)stage->inQueue->pop();
            frames[i]->stageName = stage->stageID;
        }

        
        // Pass the array to the stage function:
        Image **imgOut = (Image **)stage->fn(&frames[0]);
        
        if (imgOut) {
        
            // Push each of the results
            // TODO: for now, assumes that num input == num output
            for (int i = 0; i < stage->numFrames; i++) {
                stage->outQueue->push(imgOut[i]);
            }
        }
    }
}



/*******************
 * PIPELINE
 *******************/


Pipeline::Pipeline(size_t nStages, EStageType stageTypes[]) : m_nStages(nStages) {
    
    m_stages = new PipelineStage *[m_nStages];
    m_queues = new CQueue[nStages + 1];

    const char *stageid[] = {
        "1 demosaic", 
        "2 white bal", 
        "3 gamma correct", 
        "4 color conv", 
        "5 opt flow"
    };
    
    for (size_t i = 0; i < m_nStages; ++i) {
        m_stages[i] = new PipelineStage(stageTypes[i], stageid[i]);
        m_stages[i]->setQueues(&m_queues[i], &m_queues[i+1]);
    }
    
    m_sensor = NULL;
}


Pipeline::~Pipeline() {
    for (size_t i = 0; i < m_nStages; ++i) {
        delete m_stages[i];
    }
    delete [] m_stages;
}


void Pipeline::connectSensor(Sensor *sensor) {
    
    m_sensor = sensor;
    
    // Hook up the sensor to the first stage in the pipeline
    // by giving it access to the shared queue
    m_sensor->setQueue(&m_queues[0]);
}


bool Pipeline::powerOn() {
    
    for (size_t i = 0; i < m_nStages; ++i) {
        m_stages[i]->powerOn();
    }
    
    if (m_sensor) {
        m_sensor->powerOn();
    }
    
    return false;
}