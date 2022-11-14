#ifndef OPTION_H
#define OPTION_H

#include <stdbool.h>

#define Option(type) \
        struct { bool is_some; type value; }

#define get_value(option) \
        option.value

#define set_value(option, val) \
        option.value = val; \
        option.is_some = true;

#endif
