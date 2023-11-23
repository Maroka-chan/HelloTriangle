#include <stdint.h>
#include <vulkan/vulkan_core.h>

#include "vk_queue_family.h"
#include "../datastructures/list.h"


extern const bool ENABLE_VALIDATION_LAYERS;


VkResult create_logical_device(
                VkPhysicalDevice *p_physical_device,
                VkSurfaceKHR *p_surface,
                const char **a_device_extensions,
                uint32_t extension_count,
                const char **a_validation_layers,
                uint32_t validation_layer_count,
                VkDevice *p_device)
{
        struct QueueFamilyIndices indices =
                find_queue_families(*p_physical_device, *p_surface);

        uint32_t queueCount = 0;
        // TODO Temporary solution to avoid duplicates
        // since we do not have a Set data structure
        if (indices.graphics_family.value == indices.present_family.value)
                queueCount = 1;
        else 
                queueCount = 2;
        
        // TODO Implement List data structure
        List *queueCreateInfos = list_create(queueCount);
        // VkDeviceQueueCreateInfo queueCreateInfos[queueCount];
        
        // TODO Implement Set data structure
        // The index to the queue families are supposed to
        // only be added once if they are the same, but they will
        // be added multiple times since we are not using a Set.
        uint32_t uniqueQueueFamilies[queueCount];
        uniqueQueueFamilies[0] = indices.graphics_family.value;
        // TODO Temporary solution to avoid duplicates since
        // we do not have a Set data structure
        if (queueCount == 2)
                uniqueQueueFamilies[1] = indices.present_family.value;

        float queuePriority = 1.0f;
        for(int i = 0; i < queueCount; i++) {
                uint32_t queueFamily = uniqueQueueFamilies[i];
                VkDeviceQueueCreateInfo queueCreateInfo = {};
                queueCreateInfo.sType =
                        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                list_add(queueCreateInfos, (void*)&queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures = {};


        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = *list_get_elements(queueCreateInfos);
        createInfo.queueCreateInfoCount = queueCount;

        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = extension_count;
        createInfo.ppEnabledExtensionNames = a_device_extensions;

        if (ENABLE_VALIDATION_LAYERS) {
                createInfo.enabledLayerCount = validation_layer_count;
                createInfo.ppEnabledLayerNames = a_validation_layers;
        } else {
                createInfo.enabledLayerCount = 0;
        }


        VkResult result = vkCreateDevice(*p_physical_device,
                        &createInfo, NULL, p_device);
        if (result != VK_SUCCESS) {
                return result;
        }

        return VK_SUCCESS;
}
