#include <vulkan/vulkan_core.h>
#include <stdbool.h>

#include "string.h"

bool check_validation_layer_support(const char **validation_layers,
                int layer_count)
{
        uint32_t availableLayerCount;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);

        VkLayerProperties availableLayers[availableLayerCount];
        vkEnumerateInstanceLayerProperties(&availableLayerCount,
                        availableLayers);

        for (size_t i = 0; i < layer_count; i++) {
                bool layerFound = false;
                for (size_t j = 0; j < availableLayerCount; j++) {
                        if (strcmp(validation_layers[i],
                                        availableLayers[j].layerName) == 0) {
                                layerFound = true;
                                break;
                        }
                }
                if (!layerFound) return false;
        }

        return true;
}
