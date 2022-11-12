#ifndef OPTION_H
#define OPTION_H

#include <stdbool.h>

#define Option(type) struct { bool isSome; type value; }
#define set_option(opt, val) opt.value = val; opt.isSome = true

#endif
