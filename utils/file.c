#include "file.h"
#include <stdio.h>
#include <stdlib.h>

struct FileBytes *new_filebytes()
{
        struct FileBytes *fileBytes = malloc(sizeof(struct FileBytes));
        fileBytes->bytes = NULL;
        fileBytes->length = 0;

        return fileBytes;
}

void free_filebytes(struct FileBytes *fileBytes)
{
        free(fileBytes->bytes);
        free(fileBytes);
}

struct FileBytes *read_file(const char *filename)
{
        FILE *fileptr;
        struct FileBytes *buffer = new_filebytes();

        fileptr = fopen(filename, "rb");
        fseek(fileptr, 0, SEEK_END);
        buffer->length = ftell(fileptr);
        rewind(fileptr);

        buffer->bytes = malloc(buffer->length);
        fread(buffer->bytes, buffer->length, 1, fileptr);
        fclose(fileptr);

        return buffer;
}
