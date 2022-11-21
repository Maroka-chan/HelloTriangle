#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "vk_queue_family.h"
#include "../debug/print.h"


VkCommandPool create_command_pool(
                VkDevice *p_device,
                VkPhysicalDevice *p_physical_device,
                VkSurfaceKHR *p_surface)
{
        VkCommandPool commandPool;

        struct QueueFamilyIndices queueFamilyIndices =
                find_queue_families(*p_physical_device, *p_surface);

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex =
                queueFamilyIndices.graphics_family.value;

        if (vkCreateCommandPool(*p_device, &poolInfo, NULL, &commandPool)
                        != VK_SUCCESS) {
                error("Failed to create command pool!");
                exit(EXIT_FAILURE);
        }

        return commandPool;
}
