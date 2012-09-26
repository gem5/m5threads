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
 * test_malloc:
 * This benchmark tests malloc/free by allocating memory concurrently 
 */

static pthread_barrier_t barrier;


void*** ptr_matrix;

int iteration;

int nthreads;

typedef unsigned long int uint32;

void* alloc(void* arglist)
{
    long long int id = (long long int) arglist;

    pthread_barrier_wait(&barrier);
    
    int bytes = iteration*(id +1);
    void* ptr = malloc(bytes);
    ptr_matrix[iteration][id] = ptr;
    printf("[ALLOC %d, Thread %lld] Allocated %d bytes, from %p to %p\n", iteration, id, bytes, ptr, ((char*)ptr) + bytes - 1);

    pthread_barrier_wait(&barrier);
    int target = (id + iteration) % nthreads;
    free(ptr_matrix[iteration][target]);
    printf("[ALLOC %d, Thread %lld] Freed %d's allocation, %p\n", iteration, id, target, ptr_matrix[iteration][target]);
    //free(ptr_matrix[iteration][target]);
    return NULL;
}


int main(int argc, const char** const argv) {

    if (argc != 2) {
       printf("Usage: ./test_malloc <nthreads>\n");
       exit(1);
    }

    nthreads = atoi(argv[1]);

    pthread_t* pth = (pthread_t*) calloc(nthreads, sizeof(pthread_t));

    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);


    printf("Init done\n");

    int j;
    ptr_matrix = (void***) calloc(20, sizeof(void**));
    for(iteration = 1; iteration <= 20; iteration++) {
      pthread_barrier_init(&barrier, NULL, nthreads);
      ptr_matrix[iteration] = (void**) calloc(nthreads, sizeof(void*));
      for (j = 1; j < nthreads; j++) {
        pthread_create(&pth[j], &attr, alloc, (void*) j);
      }
      alloc((void *)0);
      for (j = 1; j < nthreads; j++) {
        pthread_join(pth[j], NULL);
      }

      pthread_barrier_destroy(&barrier);

      printf("Iteration %d completed\n", iteration);
    }
    printf("PASSED\n");
}
