#include <vulkan/vulkan_core.h>
#include <stdint.h>

#include "../option.h"
#include "vk_QueueFamilies.h"


bool isQueueFamilyIndicesComplete(
                struct QueueFamilyIndices *queuefamilyindices)
{
        return queuefamilyindices->graphicsFamily.isSome &&
                queuefamilyindices->presentFamily.isSome;
}

struct QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice,
                VkSurfaceKHR surface)
{
        struct QueueFamilyIndices indices = {};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                        &queueFamilyCount, NULL);

        VkQueueFamilyProperties queueFamilies[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
                        &queueFamilyCount, queueFamilies);

        // Find queue families that support the features we want.
        // The present and graphics family can be separate queues, but
        // are often the same. Logic can be added to prefer a physical device
        // that supports both in the same queue for improved performance.
        // TODO Implement Linked List data structure
        for (size_t i = 0; i < queueFamilyCount; i++) {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        setOption(indices.graphicsFamily, i);
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i,
                                surface, &presentSupport);
                if (presentSupport) {
                        setOption(indices.presentFamily, i);
                }

                if (isQueueFamilyIndicesComplete(&indices)) break;
        }

        return indices;
}
