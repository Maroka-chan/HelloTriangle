#ifndef OPTION_H
#define OPTION_H

#include <stdbool.h>

#define Option(type) struct { bool isSome; type value; }
#define setOption(opt, val) opt.value = val; opt.isSome = true

#endif
