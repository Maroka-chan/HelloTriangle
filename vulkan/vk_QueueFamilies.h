#ifndef VK_QUEUEFAMILIES_H
#define VK_QUEUEFAMILIES_H

#include <vulkan/vulkan_core.h>
#include "../option.h"

typedef struct QueueFamilyIndices {
  Option(uint32_t) graphicsFamily;
  Option(uint32_t) presentFamily;
} QueueFamilyIndices;

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

bool isQueueFamilyIndicesComplete(QueueFamilyIndices* queuefamilyindices);

#endif
