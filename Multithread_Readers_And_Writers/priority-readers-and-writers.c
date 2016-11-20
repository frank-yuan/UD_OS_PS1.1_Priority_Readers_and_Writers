//
//  priority-readers-and-writers.c
//  Multithread_Readers_And_Writers
//
//  Created by Xuan Yuan on 2016-11-20.
//  Copyright © 2016 Frank Yuan. All rights reserved.
//
/*
 Priority Readers and Writers
 
 Write a multi-threaded C program that gives readers priority over writers concerning a shared (global) variable. Essentially, if any readers are waiting, then they have priority over writer threads -- writers can only write when there are no readers. This program should adhere to the following constraints:
 
 Multiple readers/writers must be supported (5 of each is fine)
 Readers must read the shared variable X number of times
 Writers must write the shared variable X number of times
 
 Readers must print:
 The value read
 The number of readers present when value is read
 
 Writers must print:
 The written value
 The number of readers present were when value is written (should be 0)
 
 Before a reader/writer attempts to access the shared variable it should wait some random amount of time
 Note: This will help ensure that reads and writes do not occur all at once
 Use pthreads, mutexes, and condition variables to synchronize access to the shared variable
 */

#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "priority-readers-and-writers.h"


#define READERS_COUNT 5
#define WRITERS_COUNT 5
#define READ_TIMES 5
#define WRITE_TIMES 5

pthread_mutex_t gSharedValueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReadCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWriteCondition = PTHREAD_COND_INITIALIZER;

unsigned int gSharedValue = 0;
int gReaderCount = 0;
int gWaitingReaderCount = 0;

void *reader(void *arg){
    
    int threadId = *((int*)arg);
    
    for (int i = 0; i < READ_TIMES; ++i){
        usleep(1000 * (rand() % READERS_COUNT));
        
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
        printf(">>>>> Reader ID %d\tReaders: %d\tValue: %u\n", threadId, readCount, gSharedValue);
        
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
        usleep(1000 * (rand() % WRITERS_COUNT));
        
        pthread_mutex_lock(&gSharedValueLock);
        while (gReaderCount != 0) {
            pthread_cond_wait(&gWriteCondition, &gSharedValueLock);
        }
        gReaderCount = -1;
        pthread_mutex_unlock(&gSharedValueLock);
        
        // Perform read and output
        printf("\t<<<<< Writer ID %d\tReaders: %d\tValue: %u\n", threadId, gReaderCount < 0 ? 0 : gReaderCount, ++gSharedValue);
        
        pthread_mutex_lock(&gSharedValueLock); {
            ++gReaderCount;
            pthread_cond_signal(&gReadCondition);
        }
        pthread_mutex_unlock(&gSharedValueLock);
    }
    return (void*)threadId;
}

int main(int argc, char **argv) {
    
    srand((unsigned int)time(NULL));
    
    int readerID[READERS_COUNT];
    int writerID[WRITERS_COUNT];
    pthread_t readerThread[READERS_COUNT];
    pthread_t writerThread[WRITERS_COUNT];
    
    // create readers
    for (int i = 0; i < READERS_COUNT; ++i)
    {
        readerID[i] = i;
        pthread_create(&readerThread[i], NULL, reader, &readerID[i]);
    }
    
    // create writers
    for (int i = 0; i < WRITERS_COUNT; ++i)
    {
        writerID[i] = i;
        pthread_create(&writerThread[i], NULL, writer, &writerID[i]);
    }
    
   
    for (int i = 0; i < READERS_COUNT; ++i)
    {
        void *status;
        pthread_join(readerThread[i], &status);
        printf("[Reader ID %d Finished]\n", (int)status);
    }
    
    for (int i = 0; i < WRITERS_COUNT; ++i)
    {
        void *status;
        pthread_join(writerThread[i], &status);
        printf("[Writer ID %d Finished]\n", (int)status);
    }
    
    return 0;
}

