#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <stdlib.h>
#include <string.h>

#include "../debug/print.h"
#include "vk_queue_family.h"
#include "vk_swap_chain.h"
#include "../utils/array.h"


static bool check_device_extension_support(
                VkPhysicalDevice *p_physical_device,
                const char **device_extensions,
                uint32_t extension_count)
{
        uint32_t availableExtensionCount;
        vkEnumerateDeviceExtensionProperties(*p_physical_device, NULL,
                        &availableExtensionCount, NULL);

        VkExtensionProperties availableExtensions[availableExtensionCount];
        vkEnumerateDeviceExtensionProperties(*p_physical_device, NULL,
                        &availableExtensionCount, availableExtensions);

        // TODO Temporary solution since we do not have a Set data structure
        uint32_t matches = 0;
        for (size_t i = 0; i < extension_count; i++) {
                if (matches == extension_count)
                        break;
                for (size_t j = 0; j < availableExtensionCount; j++) {
                        if (strcmp(availableExtensions[j].extensionName,
                                                device_extensions[i])) {
                                matches++;
                                break;
                        }
                }
        }

        return matches == extension_count;
}

static bool is_device_suitable(
                VkPhysicalDevice *p_physical_device,
                VkSurfaceKHR *p_surface,
                const char **device_extensions,
                uint32_t extension_count)
{
        struct QueueFamilyIndices indices =
                find_queue_families(*p_physical_device, *p_surface);

        bool extensionsSupported =
                check_device_extension_support(p_physical_device,
                                device_extensions,
                                extension_count);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
                struct SwapChainSupportDetails swapChainSupport =
                        query_swap_chain_support(*p_physical_device, *p_surface);
                swapChainAdequate = //TODO
                        swapChainSupport.surface_format_count != 0 &&
                        swapChainSupport.present_mode_count != 0;
        }

        return 
                is_queue_family_indices_complete(&indices) &&
                extensionsSupported &&
                swapChainAdequate;
}

void pick_physical_device(
                VkInstance *p_instance,
                VkSurfaceKHR *p_surface,
                const char **device_extensions,
                uint32_t extension_count,
                VkPhysicalDevice *p_physical_device)
{
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*p_instance, &deviceCount, NULL);

        if (deviceCount == 0) {
                error("Failed to find GPUs with Vulkan support!\n");
                exit(EXIT_FAILURE);
        }

        VkPhysicalDevice devices[deviceCount];
        vkEnumeratePhysicalDevices(*p_instance, &deviceCount, devices);

        for (size_t i = 0; i < deviceCount; i++) {
                VkPhysicalDevice device = devices[i]; 
                if (is_device_suitable(&device,
                                        p_surface,
                                        device_extensions,
                                        extension_count)){
                        *p_physical_device = device;
                        break;
                }
        }

        if (p_physical_device == VK_NULL_HANDLE) {
                error("Failed to find a suitable GPU!\n");
                exit(EXIT_FAILURE);
        }
}
