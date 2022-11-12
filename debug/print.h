#ifndef PRINT_H
#define PRINT_H

#define RED "\x1B[31m"
#define YELLOW "\x1B[33m"
#define NORMAL "\x1B[0m"

#include <stdio.h>


void debug(FILE *out, const char *fmt, ...);

#define error(err_msg, ...) debug(stderr, \
                RED "[ERROR] %s: %s: %d: " err_msg NORMAL, \
                __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define warning(warn_msg, ...) debug(stderr, \
                YELLOW "[WARNING] %s: %s: %d: " warn_msg NORMAL, \
                __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define info(info_msg, ...) debug(stderr, \
                "[INFO] %s: %s: %d: " info_msg NORMAL, \
                __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#endif

