/* for-byte.c */

#include <stdio.h>
#include <stdint.h>

int main ( void )
{
  uint8_t *byte = NULL;
  uint32_t num = 0x01ABCDEF;

  /* prints 0xEF 0xCD 0xAB 0x01 (on little-endian) */
  for ( byte = (uint8_t*) &num; byte != (uint8_t*) &num + sizeof (num); byte++ )
    printf ( "0x%.2X ", *byte );
  puts ( "" );

  return 0;
}

