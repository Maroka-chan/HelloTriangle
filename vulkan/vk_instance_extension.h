#ifndef VK_INSTANCE_EXTENSION_H
#define VK_INSTANCE_EXTENSION_H

#include <stdint.h>

struct RequiredExtensions {
        const char **extensions;
        uint32_t extension_count;
};

const struct RequiredExtensions get_required_extensions();

#endif
