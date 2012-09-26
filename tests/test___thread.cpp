/*
    m5threads, a pthread library for the M5 simulator
    Copyright (C) 2009, Stanford University

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
*/



#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

// without volatile, simulator test works even if __thread support is broken
__thread volatile int local = 7;

volatile long long int jjjs = 0;

static const int count = 1024;

void* run (void* arg)
{
    long long int id = (long long int)arg;
    int i;
    printf("&local[%lld]=%p\n", id, &local);
    local += id;
    for (i = 0; i < count; i++) {
        local++;
    }

    //Some calculations to delay last read
    long long int jjj = 0;
    for (i = 0; i < 10000; i++) {
      jjj = 2*jjj +4 -i/5 + local;
    }
    jjjs = jjj;
 
    //assert(local == count +id);
    return (void*)local;
}

int main (int argc, char** argv)
{
    if (argc != 2) { 
        printf("usage: %s <thread_count>\n", argv[0]);
        exit(1);
    }
    int thread_count = atoi(argv[1]);

    printf("Starting %d threads...\n", thread_count);

    //struct timeval startTime;
    //int startResult = gettimeofday(&startTime, NULL);
    //assert(startResult == 0);
    int i;
    pthread_t* threads = (pthread_t*)calloc(thread_count, sizeof(pthread_t));
    assert(threads != NULL);
    for (i = 1 ; i < thread_count; i++) {
        int createResult = pthread_create(&threads[i], 
                                          NULL,
                                          run,
                                          (void*)i);
        assert(createResult == 0);
    }

    long long int local = (long long int)run((void*)0);
    printf("local[0] = %lld\n", local);

    for (i = 1 ; i < thread_count; i++) {
        int joinResult = pthread_join(threads[i], 
                                      (void**)&local);
        assert(joinResult == 0);
        printf("local[%d] = %lld\n", i, local);
    }
    
    /*struct timeval endTime;
    int endResult = gettimeofday(&endTime, NULL);
    assert(endResult == 0);
    
    long startMillis = (((long)startTime.tv_sec)*1000) + (((long)startTime.tv_usec)/1000);
    long endMillis   = (((long)endTime.tv_sec)*1000)   + (((long)endTime.tv_usec)/1000);
    */
    /*printf("End Time (s)    = %d\n", (int)endTime.tv_sec);
    printf("Start Time (s)  = %d\n", (int)startTime.tv_sec);
    printf("Time (s)        = %d\n", (int)(endTime.tv_sec-startTime.tv_sec));
    printf("\n");

    printf("End Time (us)   = %d\n", (int)endTime.tv_usec);
    printf("Start Time (us) = %d\n", (int)startTime.tv_usec);
    printf("Time (us)       = %d\n", (int)(endTime.tv_usec-startTime.tv_usec));
    printf("\n");

    printf("End Time (ms)   = %d\n", (int)endMillis);
    printf("Start Time (ms) = %d\n", (int)startMillis);
    printf("Time (ms)       = %d\n", (int)(endMillis-startMillis));
    printf("\n");*/

    /*double difference=(double)(endTime.tv_sec-startTime.tv_sec)+(double)(endTime.tv_usec-startTime.tv_usec)*1e-6;
    printf("Time (s) = %f\n", difference);
    printf("\n");*/
    return 0;
}
