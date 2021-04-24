#ifndef ARDUINO_UTILS_H
#define ARDUINO_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef ARDUINO

  static char print_buffer[32];

  // Using extern C so this won't work (uses C++)
  #define fwrite(fmt,...) /*sprintf(print_buffer,fmt,##__VA_ARGS__); Serial.print(print_buffer)*/

#else

  #define fwrite(fmt,...) printf(fmt,##__VA_ARGS__)

#endif

// This is to make CCLS errors go away
#ifndef __null
#define __null 0
#endif

#endif
