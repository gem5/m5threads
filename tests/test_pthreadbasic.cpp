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


#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


void* run (void* arg) {
    printf("Hello from a child thread! (thread ID: %d).\n", (int)pthread_self());
    return NULL;
}

int main(int argc, const char** argv) {
    pthread_t pth;
    pthread_attr_t attr;

    printf("Main thread initialized. TID=%d\n", (int)pthread_self());
    int result = pthread_attr_init(&attr);
    assert(result == 0);
    printf("Main thread called pthread_attr_init\n");

    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    assert(result == 0);

    printf("Main thread called pthread_attr_setscope\n");

    printf("Main thread creating 1st thread...\n");
    result = pthread_create(&pth, &attr, run, NULL);

    pthread_t pth2;
    printf("Main thread creating 2nd thread...\n");
    result = pthread_create(&pth2, &attr, run, NULL);

    printf("Main thread calling join w/ 1st thread (id=%lx)... (self=%lx)\n", pth, pthread_self());
    pthread_join(pth, NULL);
    printf("Main thread calling join w/ 2nd thread (id=%lx)... (self=%lx)\n", pth2, pthread_self());
    pthread_join(pth2, NULL);
    printf("Main thread has self=%d\n", (int)pthread_self());

    printf("Main thread done.\n");
}


