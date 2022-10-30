#include <vulkan/vulkan_core.h>
#include <stdbool.h>

#include "string.h"

bool checkValidationLayerSupport(const char** validationLayers, int layerCount) {
  uint32_t availableLayerCount;
  vkEnumerateInstanceLayerProperties(&availableLayerCount, NULL);

  VkLayerProperties availableLayers[availableLayerCount];
  vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers);

  for (size_t i = 0; i < layerCount; i++) {
    bool layerFound = false;
    for (size_t j = 0; j < availableLayerCount; j++) {
      if (strcmp(validationLayers[i], availableLayers[j].layerName) == 0) {
        layerFound = true;
        break;
      }
    }
    if (!layerFound) return false;
  }

  return true;
}
