#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void debug(FILE *p_out, const char *fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(p_out, fmt, ap);
        va_end(ap);
}

