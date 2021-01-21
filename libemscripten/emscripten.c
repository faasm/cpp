#include "faasm/emscripten.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int siprintf(char *dest, const char *format, ...) {
  int ret;
  va_list args;
  va_start(args, format);
  ret = vsprintf(dest, format, args);
  return ret;
}
