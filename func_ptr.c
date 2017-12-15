/* func_ptr.c */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int sum ( int a, int b )
{
  return a + b;
}

int main ( void )
{
  int a = 5, b = 15;

  int ( *fooptr ) // fooptr is a pointer to an int function,
    ( int, int )  // that accepts two int arguments,
    = &sum;       // and points to the address of 'sum' function

  printf ( "%d\n", fooptr ( a, b ) );

  return 0;
}

