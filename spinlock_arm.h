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


#ifndef __SPINLOCK_ARM_H__
#define __SPINLOCK_ARM_H__

static __inline__ void spin_lock (volatile int* lock) {
    unsigned long tmp;
  
         __asm__ __volatile__(
"1:     ldrex   %0, [%1]\n"
"       cmp     %0, #0\n"
"       strexeq %0, %2, [%1]\n"
"       cmpeq   %0, #0\n"
"       bne     1b\n"
"       dmb\n"
        : "=&r" (tmp)
        : "r" (lock), "r" (1)
        : "cc");

}

static __inline__ void spin_unlock (volatile int* lock) {


     __asm__ __volatile__(
"       dmb\n"
"       str     %1, [%0]\n"
        :
        : "r" (lock), "r" (0)
        : "cc");
}


static __inline__ int trylock (volatile int* lock) {
        unsigned long tmp;

        __asm__ __volatile__(
"       ldrex   %0, [%1]\n"
"       cmp     %0, #0\n"
"       strexeq %0, %2, [%1]\n"
"       eor     %0, %0, #1\n"
"       bne     fail\n"
"       dmb\n"
"fail:     nop\n"
        : "=&r" (tmp)
        : "r" (lock), "r" (1)
        : "cc", "memory");

        return tmp;
}

#endif  // __SPINLOCK_ARM_H__
