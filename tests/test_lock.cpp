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
#include <stdlib.h>
#include <stdio.h>

static pthread_rwlock_t lock;
static pthread_mutex_t trylock = PTHREAD_MUTEX_INITIALIZER;

void* run1(void* arglist)
{
    pthread_t id = pthread_self();
    printf("[run1] TID=%d\n", (int)id);

    printf("[run1] started\n");

    pthread_rwlock_rdlock(&lock);
    printf("[run1] a read lock is obtained\n");

    pthread_rwlock_unlock(&lock);
    printf("[run1] a read lock is released\n");

    return NULL;
}


void* run2(void* arglist)
{
    printf("[run2]started\n");

    int res = pthread_mutex_trylock(&trylock);
    printf("[run2] try lock result %d\n", res);

    if (res == 0) {
        pthread_mutex_unlock(&trylock);
    }
    return NULL;
}

int main(int argc, const char** const argv) {

    pthread_t pth;
    pthread_attr_t attr;
    int arg;

    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    pthread_rwlock_init(&lock, NULL);
    printf("[main]a rwlock is initialized\n");

    // test 1 : read lock 
    printf("\n1. read lock test\n");

    pthread_rwlock_rdlock(&lock);
    printf("[main]a read lock is obtained\n");

    pthread_create(&pth, &attr, run1, &arg);
    printf("[main]thread created with run1\n");

    pthread_join(pth, NULL);
    printf("[main]thread joined\n");

    pthread_rwlock_unlock(&lock);
    printf("[main]a read lock is released\n");

    // test 2 : write lock 
    printf("\n2. write lock test\n");

    pthread_rwlock_wrlock(&lock);
    printf("[main]a write lock is obtained\n");

    pthread_create(&pth, &attr, run1, &arg);
    printf("[main]thread created with run1\n");

    int i;
    for (i = 0; i < 10; i++) {
        printf("[main]idling %d\n", i);
    }

    pthread_rwlock_unlock(&lock);
    printf("[main]a write lock is released\n");

    pthread_rwlock_destroy(&lock);

    pthread_join(pth, NULL);
    printf("[main]thread joined\n");

    // test 3 : try lock 
    printf("\n3. try lock test\n");

    // 3.1 trylock will be tried to an occupied lock
    pthread_mutex_lock(&trylock);
    printf("[main]a lock is obtained\n");

    pthread_create(&pth, &attr, run2, &arg);
    printf("[main]thread created with run2\n");

    pthread_join(pth, NULL);
    printf("[main]thread joined\n");

    pthread_mutex_unlock(&trylock);
    printf("[main]a lock is released\n");

    // 3.2 trylock will be tried to a free lock
    pthread_create(&pth, &attr, run2, &arg);
    printf("[main]thread created with run2\n");

    pthread_join(pth, NULL);
    printf("[main]thread joined\n");
}
