#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_SIZE(arr) \
        (sizeof(arr) / sizeof((arr)[0]))

#define ARRAY_PTR_SIZE(arr, type) \
        (sizeof(*arr) / sizeof(type))

#endif
