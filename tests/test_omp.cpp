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

//Good old matrix multiply using openmp

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int64_t* A;
int64_t* B;
int64_t* C;

int main(int argc, const char** argv) {
  
  if (argc != 3) {
    printf("Usage: ./test_omp <nthreads> <size>\n");
    exit(1);
  }

  int nthreads = atoi(argv[1]);

  if (nthreads < 1) {
    printf("nthreads must be 1 or more\n");
    exit(1);
  }

  int size = atoi(argv[2]);
 
  if (size < 1) {
    printf("size must be 1 or more\n");
    exit(1);
  }

  printf("Setting OMP threads to %d\n", nthreads);
  omp_set_num_threads(nthreads);

  A = (int64_t*) calloc(size*size, sizeof(int64_t));
  B = (int64_t*) calloc(size*size, sizeof(int64_t));
  C = (int64_t*) calloc(size*size, sizeof(int64_t));

  printf("Starting with row/col size=%d\n",size);

  for(int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      A[x*size + y] = x*y;
    }
  }
  printf("A initialized\n");

  for(int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      B[x*size + y] = x*y - y;
    }
  }
  printf("B initialized\n");

  printf("Computing A*B with %d threads\n", nthreads);

  #pragma omp parallel for
  for(int x = 0; x < size; x++) {
    for (int y = 0; y < size; y++) {
      int64_t tot;
      for (int m = 0; m < size; m++) {
        tot += A[x*size + m]*B[m*size + y];
      }
      C[x*size + y] = tot;
    }
  }

  printf("Done\n");

  return 0;  
}


