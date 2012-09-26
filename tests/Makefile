

# ==== Variables ==============================================================

# 64-bit compiles
#Uncomment to use sparc/alpha cross-compilers
#CC := sparc64-unknown-linux-gnu-gcc
#CPP := sparc64-unknown-linux-gnu-g++
#CC := alpha-unknown-linux-gnu-gcc
#CPP := alpha-unknown-linux-gnu-g++
CC := arm-linux-gnueabi-gcc
CPP := arm-linux-gnueabi-g++
#CC := gcc
#CPP := g++

# Needed for support of v7 assembly instructions on ARM architecture
ARM_FLAGS := -march=armv7-a -marm

#CFLAGS := -ggdb3 -O3 -D__DEBUG
#CFLAGS := -g -O3 -DM5_PROFILING
CFLAGS := -g -O3 $(ARM_FLAGS)

CPPFLAGS := $(CFLAGS)

# ARM support for OpenMP not tested (test_omp.o)
TEST_OBJS := test_stackgrow.o test_pthreadbasic.o test_pthread.o test_atomic.o test_barrier.o test_lock.o test_malloc.o test_sieve.o  test___thread.o test_omp.o
#TEST_OBJS := test_stackgrow.o test_pthreadbasic.o test_pthread.o test_atomic.o test_barrier.o test_lock.o test_malloc.o test_sieve.o  test___thread.o

TEST_PROGS := $(TEST_OBJS:.o=)


# ==== Rules ==================================================================

.PHONY: default clean

default: $(TEST_PROGS) 

clean:
	$(RM)  $(TEST_OBJS) $(TEST_PROGS) $(TEST_OBJS:.o=_p) ../pthread.o

$(TEST_PROGS): $(TEST_OBJS) ../pthread.o
	$(CPP)  -static -o $@  $@.o ../pthread.o
	$(CPP)  -static -o $@_p  $@.o -lpthread

%.o: %.cpp Makefile
	$(CPP) $(CPPFLAGS)  -c -o $@ $*.cpp

#Special rules for OpenMP programs
test_omp: test_omp.o
	$(CPP)  -static -o $@  $@.o -lgomp ../pthread.o -lgomp
	$(CPP)  -static -o $@_p  $@.o -lgomp -lpthread

test_omp.o: test_omp.cpp ../pthread.o
	$(CPP) $(CPPFLAGS) -fopenmp -c -o $@ $*.cpp

../pthread.o: ../pthread.c ../pthread_defs.h ../tls_defs.h Makefile
	$(CC) $(CFLAGS) -c ../pthread.c -o ../pthread.o


