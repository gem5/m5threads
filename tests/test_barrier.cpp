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


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


pthread_barrier_t barrier;
//int A[MAX_NUM_CPUS][64];

void* run (void* arg) {
    long long int my_id = (long long int) arg;
    //A[my_id][0]++;
    printf("%lli BEFORE\n", my_id);
    pthread_barrier_wait(&barrier);
    printf("%lli AFTER\n", my_id);
    //A[my_id][0]++;
    return NULL;
}
int main (int argc, const char** const argv)  {
    int i;
    
    if (argc != 2) {
       printf("Usage: ./test_atomic <nthreads>\n");
       exit(1);
    }

    int nthreads = atoi(argv[1]);

    pthread_t* pth = (pthread_t*) calloc(nthreads, sizeof(pthread_t));

    pthread_barrier_init(&barrier, NULL, nthreads);

    for (i=1; i < nthreads; i++)  {
      pthread_create(&pth[i], NULL, run, (void*) i);
    }
    run((void*)0);
    for (i=1; i < nthreads; i++)  {
      pthread_join(pth[i], NULL);
    }

    pthread_barrier_destroy(&barrier);
    free(pth);
}
