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
        if (fileptr == NULL) {
                fprintf(stderr, "Failed to open file: %s\n", filename);
                free_filebytes(buffer);
                return NULL;
        }

        fseek(fileptr, 0, SEEK_END);
        buffer->length = ftell(fileptr);
        rewind(fileptr);

        buffer->bytes = malloc(buffer->length);
        if (fread(buffer->bytes, buffer->length, 1, fileptr) != 1) {
                fprintf(stderr, "Failed to read file: %s\n", filename);
                fclose(fileptr);
                free_filebytes(buffer);
                return NULL;
        }
        fclose(fileptr);

        return buffer;
}
