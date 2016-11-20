//
//  priority-readers-and-writers.c
//  Multithread_Readers_And_Writers
//
//  Created by Xuan Yuan on 2016-11-20.
//  Copyright © 2016 Frank Yuan. All rights reserved.
//

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "priority-readers-and-writers.h"

pthread_mutex_t gSharedValueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReadCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWriteCondition = PTHREAD_COND_INITIALIZER;

unsigned int gSharedValue = 0;
int gReaderCount = 0;
int gWaitingReaderCount = 0;

void *reader(void *arg){
    
    int threadId = *((int*)arg);
    
    for (int i = 0; i < READ_TIMES; ++i){
        usleep(1000 * (rand() % (READERS_COUNT + WRITERS_COUNT)));
        
        // Wait for conditional variables
        pthread_mutex_lock(&gSharedValueLock);
        ++gWaitingReaderCount;
        while (gReaderCount == -1) {
            pthread_cond_wait(&gReadCondition, &gSharedValueLock);
        }
        --gWaitingReaderCount;
        
        int readCount = ++gReaderCount;
        pthread_mutex_unlock(&gSharedValueLock);
        
        // Perform read and output
        printf("%u\t\tReader ID %d\tReaders: %d\n", gSharedValue, threadId, readCount);
        
        pthread_mutex_lock(&gSharedValueLock);{
            --gReaderCount;
            pthread_cond_signal(&gWriteCondition);
        }
        pthread_mutex_unlock(&gSharedValueLock);
        
    }
    return (void*)threadId;
}

void *writer(void *arg){
    int threadId = *((int*)arg);
    
    for (int i = 0; i < WRITE_TIMES; ++i){
        usleep(1000 * (rand() % (WRITERS_COUNT  + READERS_COUNT)));
        
        pthread_mutex_lock(&gSharedValueLock);
        while (gReaderCount != 0) {
            pthread_cond_wait(&gWriteCondition, &gSharedValueLock);
        }
        gReaderCount = -1;
        pthread_mutex_unlock(&gSharedValueLock);
        
        // Perform read and output
        printf("%u <<<<\tWriter ID %d\tReaders: %d\n", ++gSharedValue, threadId, gReaderCount < 0 ? 0 : gReaderCount);
        
        pthread_mutex_lock(&gSharedValueLock); {
            ++gReaderCount;
            pthread_cond_signal(&gReadCondition);
        }
        pthread_mutex_unlock(&gSharedValueLock);
    }
    return (void*)threadId;
}
