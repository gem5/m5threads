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



#ifndef __SPINLOCK_ALPHA_H__
#define __SPINLOCK_ALPHA_H__

// routines adapted from /usr/src/linux/include/asm-alpha/spinlock.h

static __inline__ void spin_lock (volatile int* lock) {
        long tmp;
        __asm__ __volatile__(
         "1:     ldl_l   %0,%1\n"
         "       bne     %0,2f\n"
         "       lda     %0,1\n"
         "       stl_c   %0,%1\n"
         "       beq     %0,2f\n"
         "       mb\n"
         ".subsection 2\n"
         "2:     ldl     %0,%1\n"
         "       bne     %0,2b\n"
         "       br      1b\n"
         ".previous"
         : "=&r" (tmp), "=m" (*lock)
         : "m"(*lock) : "memory");
}

static __inline__ void spin_unlock (volatile int* lock) {
   __asm__ __volatile__ ("mb\n");
   *lock = 0;
}

static __inline__ int trylock (volatile int* lock) {


	long regx;
	int success;

	__asm__ __volatile__(
	"1:	ldl_l	%1,%0\n"
	"	lda	%2,0\n"
	"	bne	%1,2f\n"
	"	lda	%2,1\n"
	"	stl_c	%2,%0\n"
	"	beq	%2,6f\n"
	"2:	mb\n"
	".subsection 2\n"
	"6:	br	1b\n"
	".previous"
	: "=m" (*lock), "=&r" (regx), "=&r" (success)
	: "m" (*lock) : "memory");

	return success;


}

#endif  // __SPINLOCK_H__
