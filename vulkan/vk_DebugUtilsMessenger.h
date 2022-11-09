#ifndef VALIDATIONLAYER_H
#define VALIDATIONLAYER_H

#include <vulkan/vulkan_core.h>

void setupDebugMessenger();
void populateDebugMessengerCreateInfo(
                VkDebugUtilsMessengerCreateInfoEXT *createInfo);
void destroyDebugMessenger(VkInstance instance,
                VkDebugUtilsMessengerEXT messenger,
                const VkAllocationCallbacks *pAllocator);

#endif
