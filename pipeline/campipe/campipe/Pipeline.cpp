//
//  Pipeline.cpp
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//

#include <iostream>

#include "Pipeline.h"



/*******************
 * PIPELINESTAGE
 *******************/


PipelineStage::PipelineStage(EStageType stageType, const char *stageID)
: m_stageID(stageID) {
    
    printf("Initializing stage: %s\n", m_stageID);
    m_fn = &fake_camera_algo; 
}



bool PipelineStage::powerOn() { 
    
    PStage *stage = new PStage;
    stage->inQueue = m_inQueue;
    stage->outQueue = m_outQueue;
    stage->fn = m_fn;
    stage->stageID = m_stageID;
    
    pthread_create(&m_tid, NULL, &runStage, (void *)stage);
    return true;
}


void *PipelineStage::runStage (void *vstage) {
    
    // Cast to the struct of arguments
    PStage *stage = (PStage *)vstage;
    
    while (true) {
        
        Image *fake = (Image *)stage->inQueue->pop();
        
        fake->stageName = stage->stageID;
        
        void *imgOut = stage->fn(fake);
        
       // usleep(10000);
        
        stage->outQueue->push(imgOut);
        
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
    
    m_sensor = new Sensor(12);
    m_sensor->setQueue(&m_queues[0]);
    
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
    
    m_sensor->powerOn();
    
    return false;
}