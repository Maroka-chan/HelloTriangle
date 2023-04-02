#include <stdlib.h>
#include <stdint.h>

struct _List {
    void **elements;
    uint32_t size;
    uint32_t capacity;
};

static void list_resize(struct _List *list, uint32_t new_capacity) {
    list->elements = realloc(list->elements, new_capacity * sizeof(void *));
    list->capacity = new_capacity;
}

struct _List *list_create(uint32_t initial_capacity) {
    struct _List *list = malloc(sizeof(struct _List));
    list->elements = malloc(initial_capacity * sizeof(void *));
    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

void *list_get(struct _List *list, uint32_t index) {
    if (index < 0 || index >= list->size)
        return NULL;
    return list->elements[index];
}

void list_add(struct _List *list, void *element) {
    if (list->size == list->capacity)
        list_resize(list, list->capacity * 2);

    list->elements[list->size++] = element;
}

void list_insert(struct _List *list, uint32_t index, void *element) {
    if (index < 0 || index >= list->size)
        return;

    list->elements[index] = element;
}

void list_remove(struct _List *list, uint32_t index) {
    if (index < 0 || index >= list->size)
        return;
    for (size_t i = index; i < list->size - 1; i++) {
        list->elements[i] = list->elements[i + 1];
    }
    list->size--;
}

uint32_t list_size(struct _List *list) {
    return list->size;
}

void list_free(struct _List *list) {
    free(list->elements);
    free(list);
}