#ifndef VK_QUEUEFAMILIES_H
#define VK_QUEUEFAMILIES_H

#include <vulkan/vulkan_core.h>
#include "../option.h"

struct QueueFamilyIndices {
        Option(uint32_t) graphicsFamily;
        Option(uint32_t) presentFamily;
};

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice,
                VkSurfaceKHR surface);
bool isQueueFamilyIndicesComplete(struct QueueFamilyIndices *queuefamilyindices);

#endif
