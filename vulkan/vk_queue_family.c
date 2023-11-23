#include <vulkan/vulkan_core.h>
#include <stdint.h>

#include "../option.h"
#include "vk_queue_family.h"


void create_queue(VkDevice *p_device, uint32_t queue_family, VkQueue *p_queue)
{
        vkGetDeviceQueue(*p_device, queue_family, 0, p_queue);
}

bool is_queue_family_indices_complete(
                struct QueueFamilyIndices *queue_family_indices)
{
        return queue_family_indices->graphics_family.is_some &&
                queue_family_indices->present_family.is_some;
}

struct QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device,
                VkSurfaceKHR surface)
{
        struct QueueFamilyIndices indices = {};

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                        &queueFamilyCount, NULL);

        VkQueueFamilyProperties queueFamilies[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                        &queueFamilyCount, queueFamilies);

        // Find queue families that support the features we want.
        // The present and graphics family can be separate queues, but
        // are often the same. Logic can be added to prefer a physical device
        // that supports both in the same queue for improved performance.
        // TODO Implement Linked List data structure
        for (size_t i = 0; i < queueFamilyCount; i++) {
                if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT 
                                | VK_QUEUE_GRAPHICS_BIT) {
                        set_value(indices.transfer_family, i);
                }

                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                        set_value(indices.graphics_family, i);
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i,
                                surface, &presentSupport);
                if (presentSupport) {
                        set_value(indices.present_family, i);
                }

                if (is_queue_family_indices_complete(&indices)) break;
        }

        return indices;
}
