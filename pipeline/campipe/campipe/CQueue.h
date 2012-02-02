//
//  PQueue.h
//  campipe
//
//  Created by Phaedon Sinis on 2/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef campipe_CQueue_h
#define campipe_CQueue_h


#include <queue>
#include <pthread.h>
#include <cassert>

/**
 * Concurrent queue data structure. Bundles an STL queue with a lock and condition variable.
 */
class CQueue {

public:
    
    CQueue() {
        pthread_cond_init(&m_cond, NULL);
        pthread_mutex_init(&m_lock, NULL);
    }
    
    void push(void *elem) {
        pthread_mutex_lock(&m_lock);
        m_queue.push(elem);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_lock);
    }
    
    void * pop() {
        
        void *elem = NULL;
        
        pthread_mutex_lock(&m_lock);
        while (m_queue.empty()) {
            pthread_cond_wait(&m_cond, &m_lock);
        }
        
        assert(!m_queue.empty());
        elem = m_queue.front();
        m_queue.pop();
        
        pthread_mutex_unlock(&m_lock);
        
        assert(elem != NULL);
        return elem;
    }
    
private:
    std::queue<void *> m_queue;
    pthread_cond_t m_cond;
    pthread_mutex_t m_lock;
    
};

#endif
