#ifndef VK_PHYSICAL_DEVICE_H
#define VK_PHYSICAL_DEVICE_H

#include <vulkan/vulkan_core.h>

void pick_physical_device(
                VkInstance *p_instance,
                VkSurfaceKHR *p_surface,
                const char **device_extensions,
                uint32_t extension_count,
                VkPhysicalDevice *p_physical_device);

#endif
