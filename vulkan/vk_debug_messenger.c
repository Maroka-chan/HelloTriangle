#include <vulkan/vulkan_core.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "../debug/print.h"


// Loads the vkCreateDebugUtilsMessengerEXT extension function
static VkResult create_debug_messenger(VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *p_create_info,
                VkAllocationCallbacks *p_allocator,
                VkDebugUtilsMessengerEXT *p_debug_messenger)
{
        VkResult (*functionPtr)(VkInstance,
                        const VkDebugUtilsMessengerCreateInfoEXT*,
                        const VkAllocationCallbacks*,
                        VkDebugUtilsMessengerEXT*);

        functionPtr = (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance,
                                "vkCreateDebugUtilsMessengerEXT");

        if (functionPtr != NULL) {
                return functionPtr(instance, p_create_info,
                                p_allocator, p_debug_messenger);
        } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL _debug_callback (
                VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                VkDebugUtilsMessageTypeFlagsEXT message_type,
                const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
                void *p_user_data)
{
        if (message_severity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                error("Validation Layer: %s\n", p_callback_data->pMessage);
        else if (message_severity ==
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                warning("Validation Layer: %s\n", p_callback_data->pMessage);
        else if (message_severity ==
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
                info("Validation Layer: %s\n", p_callback_data->pMessage);
        return VK_FALSE;
}

// Function for populating the vkDebugMessengerCreateInfoEXT struct.
// vkCreateDebugUtilsMessengerEXT require a valid instance and
// vkDestroyDebugUtilsMessengerEXT must be called before the
// instance is destroyed.
// 
// So we have this function so we can reuse it in createInstance in
// order to create a seperate debug utils messenger specifically for
// these functions.
// 
void populate_debug_messenger_createinfo(
                VkDebugUtilsMessengerCreateInfoEXT *p_create_info)
{
        p_create_info->sType =
                VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        p_create_info->messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        p_create_info->messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        p_create_info->pfnUserCallback = _debug_callback;
}

void setup_debug_messenger(VkInstance instance,
                VkDebugUtilsMessengerEXT *p_debug_messenger)
{
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        populate_debug_messenger_createinfo(&createInfo);

        if (create_debug_messenger(instance, &createInfo,
                                NULL, p_debug_messenger) != VK_SUCCESS) {
                error("Failed to set up debug messenger!\n");
                exit(EXIT_FAILURE);
        }
}

// Loads the vkDestroyDebugUtilsMessengerEXT extension cleanup function
void destroy_debug_messenger(VkInstance instance,
                VkDebugUtilsMessengerEXT debug_messenger,
                const VkAllocationCallbacks *p_allocator)
{
        void (*functionPtr)(VkInstance,VkDebugUtilsMessengerEXT,
                        const VkAllocationCallbacks*);

        functionPtr = (PFN_vkDestroyDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance,
                                "vkDestroyDebugUtilsMessengerEXT");

        if (functionPtr != NULL) {
                functionPtr(instance, debug_messenger, p_allocator);
        }
}
