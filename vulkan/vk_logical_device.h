#ifndef VK_LOGICAL_DEVICE_H
#define VK_LOGICAL_DEVICE_H

#include <vulkan/vulkan_core.h>

VkResult create_logical_device(
                VkPhysicalDevice *p_physical_device,
                VkSurfaceKHR *p_surface,
                const char **a_device_extensions,
                uint32_t extension_count,
                const char **a_validation_layers,
                uint32_t validation_layer_count,
                VkDevice *p_device);

#endif
