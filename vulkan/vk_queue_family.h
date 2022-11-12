#ifndef VK_QUEUE_FAMILY_H
#define VK_QUEUE_FAMILY_H

#include <vulkan/vulkan_core.h>
#include "../option.h"

struct QueueFamilyIndices {
        Option(uint32_t) graphics_family;
        Option(uint32_t) present_family;
};

struct QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device,
                VkSurfaceKHR surface);

bool is_queue_family_indices_complete(
                struct QueueFamilyIndices *queue_family_indices);

#endif
