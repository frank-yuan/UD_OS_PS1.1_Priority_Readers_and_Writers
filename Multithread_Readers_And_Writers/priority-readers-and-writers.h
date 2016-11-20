//
//  priority-readers-and-writers.h
//  Multithread_Readers_And_Writers
//
//  Created by Xuan Yuan on 2016-11-20.
//  Copyright © 2016 Frank Yuan. All rights reserved.
//

#ifndef priority_readers_and_writers_h
#define priority_readers_and_writers_h

#include <stdio.h>

#define READERS_COUNT 10
#define WRITERS_COUNT 2
#define READ_TIMES 10
#define WRITE_TIMES 8


void *reader(void *arg);
void *writer(void *arg);
#endif /* priority_readers_and_writers_h */
