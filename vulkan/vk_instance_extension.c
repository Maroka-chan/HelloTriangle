#include <stdbool.h>
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "../utils/array.h"
#include "vk_instance_extension.h"

extern const bool ENABLE_VALIDATION_LAYERS;

const struct RequiredExtensions get_required_extensions()
{
        struct RequiredExtensions glfwExtensions = {};
        glfwExtensions.extensions =
                glfwGetRequiredInstanceExtensions(
                                &glfwExtensions.extension_count);

        // Enable extra extensions when validation layers are enabled
        if (ENABLE_VALIDATION_LAYERS) {
                const char *extraExtenstions[] = {
                        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
                };

                size_t extraExtensionsSize = ARRAY_SIZE(extraExtenstions);
                struct RequiredExtensions extensions = {};

                extensions.extension_count =
                        glfwExtensions.extension_count + extraExtensionsSize;
                extensions.extensions = malloc(extensions.extension_count);             // TODO Call free somewhere

                // Merge the arrays
                size_t i,j;
                for (i = 0; i < glfwExtensions.extension_count; i++)
                        extensions.extensions[i] =
                                glfwExtensions.extensions[i];

                for (i = 0, j = glfwExtensions.extension_count;
                                j < extensions.extension_count &&
                                i < extraExtensionsSize; i++, j++)
                        extensions.extensions[j] = extraExtenstions[i];

                return extensions;
        }

        return glfwExtensions;
}
