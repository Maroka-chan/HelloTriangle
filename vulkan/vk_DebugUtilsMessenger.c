#include <vulkan/vulkan_core.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "../debug/print.h"


// Loads the vkCreateDebugUtilsMessengerEXT extension function
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                VkAllocationCallbacks *pAllocator,
                VkDebugUtilsMessengerEXT *pDebugMessenger)
{
        VkResult (*functionPtr)(VkInstance,
                        const VkDebugUtilsMessengerCreateInfoEXT*,
                        const VkAllocationCallbacks*,
                        VkDebugUtilsMessengerEXT*);

        functionPtr = (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance,
                                "vkCreateDebugUtilsMessengerEXT");

        if (functionPtr != NULL) {
                return functionPtr(instance, pCreateInfo,
                                pAllocator, pDebugMessenger);
        } else {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
}

// Loads the vkDestroyDebugUtilsMessengerEXT extension cleanup function
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                VkDebugUtilsMessengerEXT debugMessenger,
                const VkAllocationCallbacks *pAllocator)
{
        void (*functionPtr)(VkInstance,VkDebugUtilsMessengerEXT,
                        const VkAllocationCallbacks*);

        functionPtr = (PFN_vkDestroyDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance,
                                "vkDestroyDebugUtilsMessengerEXT");

        if (functionPtr != NULL) {
                functionPtr(instance, debugMessenger, pAllocator);
        }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData)
{
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                error("Validation Layer: %s\n", pCallbackData->pMessage);
        else if (messageSeverity ==
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                warning("Validation Layer: %s\n", pCallbackData->pMessage);
        else if (messageSeverity ==
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
                info("Validation Layer: %s\n", pCallbackData->pMessage);
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
void populateDebugMessengerCreateInfo(
                VkDebugUtilsMessengerCreateInfoEXT *createInfo)
{
        createInfo->sType =
                VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo->messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo->messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo->pfnUserCallback = debugCallback;
}

void setupDebugMessenger(VkInstance instance,
                VkDebugUtilsMessengerEXT *debugMessenger)
{
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        populateDebugMessengerCreateInfo(&createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo,
                                NULL, debugMessenger) != VK_SUCCESS) {
                error("Failed to set up debug messenger!\n");
                exit(EXIT_FAILURE);
        }
}

void destroyDebugMessenger(VkInstance instance,
                VkDebugUtilsMessengerEXT debugMessenger,
                const VkAllocationCallbacks *pAllocator)
{
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, pAllocator);
}


