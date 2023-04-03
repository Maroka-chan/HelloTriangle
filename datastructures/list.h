#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct _List List;

struct _List *list_create(uint32_t initial_capacity);
void *list_get(struct _List *list, uint32_t index);
void **list_get_elements(struct _List *list);
void list_add(struct _List *list, void *element);
void list_insert(struct _List *list, uint32_t index, void *element);
void list_remove(struct _List *list, uint32_t index);
uint32_t list_size(struct _List *list);
void list_free(struct _List *list);

#endif
