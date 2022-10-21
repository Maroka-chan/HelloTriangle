#ifndef DEBUG_H
#define DEBUG_H

#define RED "\x1B[31m"
#define YELLOW "\x1B[33m"
#define NORMAL "\x1B[0m"

#include <stdio.h>
void debug(FILE *out, const char *fmt, ...);
void error(const char *err_msg);
void warning(const char *warn_msg);
void info(const char *info_msg);

#define error(err_msg, ...) debug(stderr, RED "[ERROR] %s: %s: %d: " err_msg NORMAL, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define warning(warn_msg, ...) debug(stderr, YELLOW "[WARNING] %s: %s: %d: " warn_msg NORMAL, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define info(info_msg, ...) debug(stderr, "[INFO] %s: %s: %d: " info_msg NORMAL, __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#endif

