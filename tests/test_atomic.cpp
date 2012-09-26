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

    Author: Daniel Sanchez
*/


#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * test_atomic:
 * This benchmark is intended to stress-test atomic operations by using heavily 
 * contended locks. 
 */

static pthread_mutex_t lock;
static pthread_barrier_t barrier;

int* intArray;
int next;

int iteration;

void* chain(void* arglist)
{
  int iteration;
  for(iteration = 1; iteration <= 10; iteration++) {
    long long int id = (long long int) arglist;
    pthread_barrier_wait(&barrier);

    pthread_mutex_lock(&lock);

    int current = next;
    printf("[Iteration %d, Thread %lld] Got lock\n", iteration, id);
    intArray[current]++;

    //Uncomment this snip for longer-running critical section
    /*int i;
    for (i = 0; i < 5000; i++) {
      next = (i + next)/2;
      Sim_Print0(""); //so that gcc does not optimize this out
    }*/

    next = id;

    printf("[Iteration %d, Thread %lld] Critical section done, previously next=%d, now next=%d\n", iteration, id, current, next);
    pthread_mutex_unlock(&lock);

    pthread_barrier_wait(&barrier);
  }

    return NULL;
}


int main(int argc, const char** const argv) {
    if (argc != 2) {
       printf("Usage: ./test_atomic <nthreads>\n");
       exit(1);
    }
    int nthreads = atoi(argv[1]);

    if (nthreads < 2) {
        printf("\nthis test requires at least 2 cpus\n");
        exit(0);
    }

    pthread_t pth[nthreads];
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);


    pthread_mutex_init(&lock, NULL);
    printf("Init done\n");

    int j;

    //for(iteration = 1; iteration <= 10; iteration++) {
      pthread_barrier_init(&barrier, NULL, nthreads);
      intArray = (int*) calloc(nthreads, sizeof(int));
      next = 0;
      for (j = 1; j < nthreads; j++) {
        pthread_create(&pth[j], &attr, chain, (void*) j);
      }
   
   
    for(iteration = 1; iteration <= 10; iteration++) {
      pthread_barrier_wait(&barrier);
      /*for (j = 1; j < Sim_GetNumCpus(); j++) {
        pthread_join(pth[j], NULL);
      }*/
      pthread_barrier_wait(&barrier);

      intArray[next]++;
      int failed = 0;
      for (j = 0; j < nthreads; j++) {
        if (intArray[j] != 1) {
          printf("FAILED: Position %d had %d instead of 1\n", j, intArray[j]);
          failed = 1;
        }
      }
      if (failed) exit(1);
      //pthread_barrier_destroy(&barrier);
      //free(intArray);
      for (j = 0; j < nthreads; j++) intArray[j] = 0;
      next = 0;
      //intArray = (int*) calloc(Sim_GetNumCpus(), sizeof(int));

      printf("Iteration %d completed\n", iteration);
    }

    printf("PASSED :-)\n");
}
