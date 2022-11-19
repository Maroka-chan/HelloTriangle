#ifndef FILE_H
#define FILE_H

#include <stdint.h>

struct FileBytes {
        char *bytes;
        uint32_t length;
};

void free_filebytes(struct FileBytes *filebytes);
struct FileBytes *read_file(const char *filename);

#endif
