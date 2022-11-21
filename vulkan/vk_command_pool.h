#ifndef VK_COMMAND_POOL_H
#define VK_COMMAND_POOL_H

#include <vulkan/vulkan_core.h>

VkCommandPool create_command_pool(
                VkDevice *p_device,
                VkPhysicalDevice *p_physical_device,
                VkSurfaceKHR *p_surface);

#endif
