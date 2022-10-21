#ifndef DEBUG_H
#define DEBUG_H

#define RED "\x1B[31m"
#define NORMAL "\x1B[0m"

#include <stdio.h>
void debug(FILE *out, const char *fmt, ...);
void error(const char *err_msg);

#define error(err_msg) debug(stderr, RED "[ERROR] %s: %s: %d: %s" NORMAL, __FILE__, __func__, __LINE__, err_msg)

#endif

