m5threads -- A pthread library for the M5 simulator
===================================================
Daniel Sanchez, Stanford University


Changelog
---------

14-Feb-09
- Added support for OpenMP in SPARC.
- Fixed stack guard to work in SPARC64 (stack bias was insufficient).
- Added optional profiling hooks to measure synchronization use. Compile with -DM5_PROFILING to use M5 profiling syscalls.
- The Makefile now builds test programs linked with both m5threads (test_XXX) and the standard pthread library (test_XXX_p). This is done for debugging purposes, but note that **the _p binaries won't work in M5**.

27-Jan-09
- Added support for TLS in SPARC and x86-64 in static binaries. Alpha no longer works due to having unimplemented TLS support.
- Fixed a race condition in rwlocks and condition variables.
- Added support for detached threads.
- Added thread-specific data (TSD) functions: key_create/delete/getspecific/setspecific.
- Integrated with NPTL/LinuxThreads-based glibc (libc aliases, specific functions, intialization routines, and libc-specific TSD). libc calls are now MT-safe and the library runs the full test suite correctly. Tested on SPARC/glibc2.3.6/Linux 2.6.11 (LinuxThreads) in M5, and x86-64/glibc2.6/Linux 2.6.26 (NPTL) in an 8-core machine.
- Added support for OpenMP programs (see test_opm.cpp) -- for now, works in x86 (real machine and M5), but not in SPARC.
- Licensed under GPLv2.
- Extended this README.

23-Jan-2009 
 - Added support for SPARC in pthread_exit
 - Substituted tree barriers by counter barriers. Now barriers work regardless of which threads take them.

21-Jan-2009
 - Initial version

License
-------

This software is licensed under the GPLv2. See the LICENSE file for a full copy of the license.

This software contains portions of code from the Linux kernel and glibc 2.3.6. Both are redistributed under the terms of the GPL.

Description
-----------

This library enables M5 to simulate multithreaded apps in system call emulation mode. It is intended as a replacement of NPTL/LinuxThreads implementations of libpthread. Instead of using a large portion of the Linux system calls, this library does as much as possible in user-level code. It requires just two system calls: clone, to spawn a new process, and exit, to finish a thread. As a result, it is easy to support in a syscall-emulation simulator. However, this is not a full implementation of pthreads, and the library lacks a thread scheduler. In M5, you will not be able to schedule more threads than thread contexts. 

This library works in M5, but in real systems too. Both x86-64 and SPARC systems running Linux should execute programs correctly. In real systems, you will be able to allocate more threads than CPUs, but performance will degrade in this case since there is no thread scheduler (and thread switching occurs at the granularity of the OS scheduler).

Only a subset of the pthread specification is supported. This includes:
- Creation and join of joinable and detached threads; pthread_exit
- Regular mutexes (NOT recursive or other rare modes)
- Regular read-write locks
- Barriers
- Condition variables
- Keys (key_create/delete, get/setspecific)
- Miscellaneous functions:
In particular, the following thinks are not supported:
- pthread_cancel and related functions
- pthread_kill
- Anything else that has to do with signals
- pthread_cleanup_XXX, pthread_unwind
If your program uses a non-implemented pthread function, it will fail an assertion.

This library should compile with GNU toolchains implementing LinuxThreads (Linux <=2.4 or 2.6) or NPTL (2.6 only) pthreads. If you compile it with an NPTL glibc, you may get futex() system calls if you try to do concurrent calls to multithreaded-safe glibc functions (e.g. printf). These are unimplemented in M5. To avoid them, enclose these calls in a global lock. Additionally, NPTL apps tend to use more system calls, so it is recommended to use M5 with a glibc compiled with LinuxThreads. Performance should be practically identical with both versions, as we are substituting the threading library.

This library includes support for thread-local storage (TLS), but only for the SPARC and x86-64 ABIs (which are nearly identical). Alpha is no longer supported. Supporting Alpha would require implementing its TLS ABI.

Compiling & using
-----------------

Applications compiled with this library should be built statically, and should link against the built pthread.o object file. Again, see the Makefile in the tests/ directory for the exact commands used.

By default, the tests/Makefile builds all the tests using your system's g++. You can build sparc binaries by building a cross-compiler.

TODOs
-----
- Ticket/MCS locks
- Tree barriers
- Add a scheduler, turning the library to an M:N model

Implementation details
======================

What follows is recommended reading if you want to understand how the library works in more detail and extend it.

This library implements mutexes as TTS spinlocks (taken from the Linux source code tree), and barriers as counter barriers. Compatibility with NPTL and LinuxThreads data structures is maintained by using a variety of macros, defined in pthread_defs.h

All the memory regions needed to spawn a thread are contained in a single memory segment, the thread block, which has the following format:

---------------------- <- lower addresses
 Thread Control Block
----------------------
      TLS data
---------------------- <- TLS pointer
 (Unused) "real" TCB
----------------------
    [empty space]
----------------------
        Stack
---------------------- <- Initial stack pointer (grows to lower addresses)
     Stack Guard
---------------------- <- upper addresses

The thread control block contains the information relative to the current thread (status, flags, etc). The thread ID (returned by pthread_self()) is simply a pointer to the TCB. This enables distributed thread create/join, and there are no global structures for tracking thread data.

The "real" TCB is the TCB defined by LinuxThreads or NPTL. We don't use it directly and initialize its contents to 0, but reserve some space because some variables (most notably errno) are in this area.

The thread block conforms to the TLS ABI for x86-64 and SPARC architectures, which follows variant II as described in http://people.redhat.com/drepper/tls.pdf If you wish to extend this to other architectures, e.g. Alpha, be sure to read sections 1-3 of that document. Also, most of the code for the TLS part of the library is taken from glibc2.3.6 (search for the __libc_setup_tls function).

In pthread_create, the parent mmaps the thread block, populates the TCB, and spawns a new child. The child sets up TLS before starting execution. In joinable threads, it is the responsibility of the parent to munmap the thread block. If the thread is detached, it will munmap its own thread block when exiting. The thread block is allocated with mmap because 1) it is a sizable chunk of memory and 2) this way, the child can delete its own stack (since munmap is a system call, not a function call, a stack is not required on return).

The library includes function aliases, extra definitions, libc-specific keys and initialization code to work correctly with glibc. When linking with this library, glibc function calls *should* be MT-safe. However, how libc and libpthread interact has changed over time, and this may not work correctly with glibc versions >2.6. Modifying the code to support other versions of glibc should be straightworward, if you have some idea for what you are doing. I recommend using nm (to see what symbols are defined) and objdump (to dissasemble) on whatever version of libc.a you'll be using, and see if there is any mismatch with the "glibc glue" code in the library (mostly, at the end of pthread.c).


