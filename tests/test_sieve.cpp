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


/*
   g++ -O3 -o ./sieve -lm -lpthread sieve.cpp && time sieve 1 && time sieve 2
*/

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>


static int  max_limit;
static int  sqrt_max_limit;
static int* not_prime;

static const int print = 1;

static const int start = 2;
static int sqrt_limit;
static int limit;

static void* run (void*);
#ifdef SIMULATOR 
void mainX(int argc, const char **argv, const char **envp)
#else
int main (int argc, char** argv)
#endif
{
#ifdef SIMULATOR
    if (argc != 1) { 
        printf("usage: %s\n", argv[0]);
        exit(1);
    }
    int thread_count = Sim_GetNumCpus();
#else
    if (argc != 2) { 
        printf("usage: %s <thread_count>\n", argv[0]);
        exit(1);
    }
    int thread_count = atoi(argv[1]);
#endif

#ifdef SIMULATOR
    max_limit      =  10000;
#else
    max_limit      =  10000;//0000;
#endif
    sqrt_max_limit = (int)ceil(sqrt(max_limit));

    not_prime      = (int*)calloc(max_limit, sizeof(int));
    assert(not_prime != NULL);

    sqrt_limit = (int)ceil(sqrt(sqrt_max_limit));
    limit      =                sqrt_max_limit;
    if (1) {
        not_prime[0] = 1;
        not_prime[1] = 1;
        run(NULL);
    }

    sqrt_limit = (int)ceil(sqrt(max_limit));
    limit      =                max_limit;

    printf("sqrt_max_limit %d\n", sqrt_max_limit);
    printf("max_limit      %d\n", max_limit);
#ifdef SIMULATOR
    printf("Starting threads...\n");
#else
    printf("Starting %d threads...\n", thread_count);
#endif

#ifndef SIMULATOR
    struct timeval startTime;
    int startResult = gettimeofday(&startTime, NULL);
    assert(startResult == 0);
#endif


    pthread_t* threads = (pthread_t*)calloc(thread_count, sizeof(pthread_t));
    assert(threads != NULL);
    for (int i = 1 ; i < thread_count; i++) {
        int createResult = pthread_create(&threads[i], 
                                          NULL,
                                          run,
                                          NULL);
        assert(createResult == 0);
    }

    run(NULL);

    for (int i = 1 ; i < thread_count; i++) {
        int joinResult = pthread_join(threads[i], 
                                      NULL);
        assert(joinResult == 0);
    }
    
#ifndef SIMULATOR
    struct timeval endTime;
    int endResult = gettimeofday(&endTime, NULL);
    assert(endResult == 0);
    
    long startMillis = (((long)startTime.tv_sec)*1000) + (((long)startTime.tv_usec)/1000);
    long endMillis   = (((long)endTime.tv_sec)*1000)   + (((long)endTime.tv_usec)/1000);

    printf("%d\n", (int)endTime.tv_sec);
    printf("%d\n", (int)startTime.tv_sec);
    printf("%d\n", (int)(endTime.tv_sec-startTime.tv_sec));
    printf("\n");

    printf("%d\n", (int)endTime.tv_usec);
    printf("%d\n", (int)startTime.tv_usec);
    printf("%d\n", (int)(endTime.tv_usec-startTime.tv_usec));
    printf("\n");

    printf("%d\n", (int)endMillis);
    printf("%d\n", (int)startMillis);
    printf("%d\n", (int)(endMillis-startMillis));
    printf("\n");

    double difference=(double)(endTime.tv_sec-startTime.tv_sec)+(double)(endTime.tv_usec-startTime.tv_usec)*1e-6;
    printf("%f\n", difference);
    printf("\n");
#endif

    if (print) {
        printf("Primes less than 100:\n");
        for (int i = 0; i < 100; i++) {
            if (!not_prime[i]) {
                printf("%d\n",i);
            }
        }
    }

#ifndef SIMULATOR
    return 0;
#endif
}

void* run (void* arg)
{
    if (0 /*Sim_GetMode() == MODE_TM*/) {

#ifdef WITH_TM
        for (int my_prime = start; my_prime < sqrt_limit; ++my_prime) {
            if (!not_prime[my_prime]) {
                TM_BeginClosed(); {
                    not_prime[my_prime] = true;
                }
                TM_EndClosed();
                for (int multiple = my_prime*2; multiple < limit; multiple += my_prime) {
                    not_prime[multiple] = true;
                }
                TM_BeginClosed(); {
                    not_prime[my_prime] = false;
                }
                TM_EndClosed();
            }
        }
#else
        printf("Somehow mode is MODE_TM but WITH_TM was not defined.");
        exit(1);
#endif
    }
    else {
        for (int my_prime = start; my_prime < sqrt_limit; ++my_prime) {
            if (!not_prime[my_prime]) {
                // Sim_Print1("Found prime: %d\n", my_prime);
                not_prime[my_prime] = 1;
                for (int multiple = my_prime*2; multiple < limit; multiple += my_prime) {
                    not_prime[multiple] = 1;
                }
                not_prime[my_prime] = 0;
            } else {
                // Sim_Print1("not prime: %d\n", my_prime);
            }
        }
    }
    return NULL;
}
