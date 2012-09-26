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



/* Just test which direction the stack grows in this arch/ABI
   Kind of a big deal when deciding which end of the stack to
   pass as a pointer to clone :-)
*/

#include <stdio.h>


void func (int* f1) {
  int f2;
  printf("Addr frame 1 = %p, Addr frame 2 = %p\n", f1, &f2);
  if (&f2 > f1) {
    printf("Stack grows up (and this threading library needs to be fixed for your arch...)\n");
  } else {
    printf("Stack grows down\n");
  }
}

int main (int argc, char**argv) {
  int f1;
  func(&f1);
}

