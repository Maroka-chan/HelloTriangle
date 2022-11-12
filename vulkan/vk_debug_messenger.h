#ifndef VK_DEBUG_MESSENGER_H
#define VK_DEBUG_MESSENGER_H

#include <vulkan/vulkan_core.h>

void setup_debug_messenger(VkInstance instance,
                VkDebugUtilsMessengerEXT *p_debug_messenger);

void populate_debug_messenger_createinfo(
                VkDebugUtilsMessengerCreateInfoEXT *p_create_info);

void destroy_debug_messenger(VkInstance instance,
                VkDebugUtilsMessengerEXT messenger,
                const VkAllocationCallbacks *p_allocator);

#endif
