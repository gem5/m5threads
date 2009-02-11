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
#include <stdlib.h>

#include <stdio.h>

void* run1(void* arglist)
{
    int* args = (int*)arglist;
    printf("[run1] argument passed %d %d\n", args[0], args[1]);

    // yield() does nothing
    pthread_yield();

    pthread_t run1th = pthread_self();
    printf("[run1] thread id : %d \n", (int)run1th);

    pthread_exit(0);
    assert(false);
    return NULL;
}

int gl_counter = 0;

void funcWithCriticalSection()
{
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    int result = pthread_mutex_lock(&lock);
    assert(result == 0);
    gl_counter++;
    printf("%d\n",gl_counter);
    result = pthread_mutex_unlock(&lock);
    assert(result == 0);
}

void* run2(void* arglist)
{
    int  i;
    for (i = 0; i <5; i++) {
        funcWithCriticalSection();
    }
    return NULL;
}

pthread_key_t key;

void* run3(void* arglist)
{
    int j;
    int i = pthread_self();
    int result = pthread_setspecific(key, (void*)&i);
    assert(result == 0);
    void* value = pthread_getspecific(key);
    assert(value != NULL);

    for (j=5; j >= 0 ; j--) {
        printf("");
    }
    
    if (*((int*)value) == (int) pthread_self()) {
        printf("[run3]thread-private value matches 2 : %d \n", *((int*)value));
    }
    else {
        printf("[run3]thread-private value doesn't match 2 : %d \n", *((int*)value));
    }
    return NULL;
}

pthread_cond_t cond_sync = PTHREAD_COND_INITIALIZER;
pthread_mutex_t cond_lock = PTHREAD_MUTEX_INITIALIZER;

void* run4(void* arglist)
{
    int j;
    for (j=5; j >= 0 ; j--) {
        printf("[run4] goofing around for a moment %d \n", j);
    }

    int result = pthread_mutex_lock(&cond_lock);
    assert(result == 0);

    printf("[run4] about to call broadcast\n"); {
        result = pthread_cond_broadcast(&cond_sync);
        assert(result == 0);
    }

    result = pthread_mutex_unlock(&cond_lock);
    assert(result == 0);
    return NULL;
}

int main(int argc, const char** const argv) {

    // test 1 : creation & join
    printf("\n1. thread creation and join test\n");

    pthread_t pth;
    pthread_attr_t attr;
    int arg[2];

    arg[0] = 2; 
    arg[1] = 4; 

    int result = pthread_attr_init(&attr);
    assert(result == 0);
    result = pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    assert(result == 0);

    result = pthread_create(&pth, &attr, run1, &arg);
    assert(result == 0);
    printf("[main]thread(%d) created with run1\n", (int)pth);

    result = pthread_join(pth, NULL);
    assert(result == 0);
    printf("[main]thread(%d) joined\n", (int)pth);
    

    // test 2 : mutex   
    printf("\n2. mutex test\n");
    result = pthread_create(&pth, &attr, run2, &arg);
    assert(result == 0);
    printf("[main]thread(%d) created with run2\n", (int)pth);
    
    int i;
    for (i = 0; i <5; i++) {
        funcWithCriticalSection();
    }

    result = pthread_join(pth, NULL);
    assert(result == 0);


    // test 3 : key 
    printf("\n3. thread-private storage test\n");
    result = pthread_key_create(&key, NULL);
    assert(result == 0);

    result = pthread_create(&pth, &attr, run3, &arg);
    assert(result == 0);

    printf("[main]thread(%d) created with run3\n", (int)pth);

    i = 1;
    result = pthread_setspecific(key, (void*)&i);
    assert(result == 0);
    void* value = pthread_getspecific(key);
    assert(value != NULL);

    if (*((int*)value) == 1) {
        printf("[main]thread-private value matches 1 : %d \n", *((int*)value));
    }
    else {
        printf("[main]thread-private value doesn't match 1 : %d \n", *((int*)value));
    }

    result = pthread_join(pth, NULL);
    assert(result == 0);


    // test 4 : wait / notifyall
    printf("\n4. wait-notifyall test\n");
    result = pthread_create(&pth, &attr, run4, &arg);
    assert(result == 0);
    printf("[main]thread(%d) created with run4\n", (int)pth);

    result = pthread_mutex_lock(&cond_lock);
    assert(result == 0);

    printf("[main]going into wait()\n");
    result = pthread_cond_wait(&cond_sync, &cond_lock);
    assert(result == 0);

    result = pthread_mutex_unlock(&cond_lock);
    assert(result == 0);

    result = pthread_join(pth, NULL);
    assert(result == 0);
}
