#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "../option.h"
#include "../debug/print.h"
#include "vk_QueueFamilies.h"
#include "vk_SwapChain.h"


struct SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                VkSurfaceKHR surface)
{
        struct SwapChainSupportDetails details = {};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, 
                        &details.capabilities);

        uint32_t surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, 
                        &surfaceFormatCount, NULL);

        details.surfaceFormatCount = surfaceFormatCount;
        if (surfaceFormatCount != 0) {
                details.formats = 
                        malloc(surfaceFormatCount * 
                                        sizeof(VkSurfaceFormatKHR));
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, 
                                &surfaceFormatCount, details.formats);
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
                        &presentModeCount, NULL);

        details.presentModeCount = presentModeCount;
        if (presentModeCount != 0) {
                details.presentModes = 
                        malloc(presentModeCount * sizeof(VkPresentModeKHR));
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
                                &presentModeCount, details.presentModes);
        }

        return details;
}


// Color Depth
// Chooses the color format and colorspace
static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
                const VkSurfaceFormatKHR *availableFormats,
                uint32_t formatCount)
{
        for (size_t i = 0; i < formatCount; i++) {
                if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
                                availableFormats[i].colorSpace 
                                == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        return availableFormats[i];
                }
        }

        return availableFormats[0];
}


// Conditions for "swapping" images to the screen
//
// For example, VK_PRESENT_MODE_IMMEDIATE_KHR submits images to
// the screen immediately which can cause screen tearing.
//
// VK_PRESENT_MODE_FIFO_KHR we use a first in first out(FIFO) queue
// and if the queue is full the program has to wait.
//
// VK_PRESENT_MODE_FIFO_RELAXED_KHR is like FIFO_KHR, but transfers the
// image immediately instead of waiting if the last vertical blank was empty.
//
// VK_PRESENT_MODE_MAILBOX_KHR is like FIFO_KHR but instead of waiting when
// the queue is full, it overwrites the already queued images with newer ones.
// This renders frames as fast as possible while still avoiding screen tearing
// and results in fewer latency issues than standard vertical sync.
// It is not as power efficient since the program never stops to wait,
// so it is not the best choice where energy usage is
// more important like on a mobile device.
static VkPresentModeKHR chooseSwapPresentMode(
                const VkPresentModeKHR *availablePresentModes, 
                uint32_t presentModeCount)
{
        for (size_t i = 0; i < presentModeCount; i++) {
                if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        return availablePresentModes[i];
                }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
}


double clamp(double d, double min, double max)
{
        const double t = d < min ? min : d;
        return t > max ? max : t;
}

static VkExtent2D chooseSwapExtent(GLFWwindow *window,
                VkSurfaceCapabilitiesKHR capabilities)
{
        if (capabilities.currentExtent.width != UINT32_MAX) {
                return capabilities.currentExtent;
        } else {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);

                VkExtent2D actualExtent = {
                        (uint32_t) width,
                        (uint32_t) height
                };

                actualExtent.width = clamp(actualExtent.width, 
                                capabilities.minImageExtent.width,
                                capabilities.maxImageExtent.width);
                actualExtent.height = clamp(actualExtent.height,
                                capabilities.minImageExtent.height,
                                capabilities.maxImageExtent.height);

                return actualExtent;
        }
}

VkSwapchainCreateInfoKHR create_swapchain_info(
                VkPhysicalDevice physicalDevice,
                VkSurfaceKHR surface,
                VkSurfaceFormatKHR surfaceFormat,
                uint32_t swapChainImageCount,
                VkExtent2D extent,
                struct SwapChainSupportDetails swapChainSupport,
                VkPresentModeKHR presentMode)
{
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = swapChainImageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;                                                // The imageArrayLayers specifies the amount of layers each image consists of.
                                                                                        // This is always 1 unless you are developing a stereoscopic 3D application
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;                    // Change if you want to do post-processing etc.

        struct QueueFamilyIndices indices =
                findQueueFamilies(physicalDevice, surface);

        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value,
                indices.presentFamily.value };

        if (indices.graphicsFamily.value != indices.presentFamily.value) {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0; // Optional
                createInfo.pQueueFamilyIndices = NULL; // Optional
        }

        createInfo.preTransform =
                swapChainSupport.capabilities.currentTransform;                         // currentTransform tells that we do not want any transformation
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;                  // Ignore alpha channel, so that it does not blend with other windows in the window system
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;                                                   // We do not care about the color of obscured pixels, for example because another window is in front of them
        createInfo.oldSwapchain = VK_NULL_HANDLE;                                       // Assume we'll only ever create one swap chain for now

        return createInfo;
}

struct SwapChainDetails createSwapChain(GLFWwindow *window, VkDevice device,
                VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
        struct SwapChainSupportDetails swapChainSupport =
                querySwapChainSupport(physicalDevice, surface);

        VkSurfaceFormatKHR surfaceFormat =
                chooseSwapSurfaceFormat(swapChainSupport.formats,
                                swapChainSupport.surfaceFormatCount);

        VkPresentModeKHR presentMode =
                chooseSwapPresentMode(swapChainSupport.presentModes,
                                swapChainSupport.presentModeCount);

        VkExtent2D extent = chooseSwapExtent(window,
                        swapChainSupport.capabilities);

        // We should request at least one more than the minimum to avoid
        // having to wait for the driver to complete internal operations
        // before we can acquire another image to render to.
        uint32_t swapChainImageCount =
                swapChainSupport.capabilities.minImageCount + 1;

        // Make sure we do not exceed the maximum image count.
        // We first check that the maximum is larger than 0 since
        // 0 is a special value that means that there is no maximum.
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
                        swapChainImageCount >
                        swapChainSupport.capabilities.maxImageCount) {

                swapChainImageCount =
                        swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo =
                create_swapchain_info(physicalDevice, surface,
                                surfaceFormat, swapChainImageCount,
                                extent, swapChainSupport, presentMode);

        VkSwapchainKHR swapChain;
        if (vkCreateSwapchainKHR(device, &createInfo,
                                NULL, &swapChain) != VK_SUCCESS) {

                error("Failed to create swap chain!\n");
                exit(EXIT_FAILURE);
        }

        vkGetSwapchainImagesKHR(device, swapChain,
                        &swapChainImageCount, NULL);
        VkImage *swapChainImages =
                malloc(swapChainImageCount * sizeof(VkImage));
        vkGetSwapchainImagesKHR(device, swapChain,
                        &swapChainImageCount, swapChainImages);

        VkFormat swapChainImageFormat = surfaceFormat.format;
        VkExtent2D swapChainExtent = extent;

        struct SwapChainDetails details = {};
        details.swapChain = &swapChain;
        details.swapChainExtent = swapChainExtent;
        details.swapChainImageFormat = swapChainImageFormat;
        details.swapChainImages = swapChainImages;
        details.swapChainImageCount = swapChainImageCount;

        return details;
}


void destroySwapChainSupportDetails(struct SwapChainSupportDetails *details)                   // TODO Destroy somewhere. Probably save them in a global list and destroy on Cleanup()??
{
        if (details->surfaceFormatCount != 0)
                free(details->formats);
        if (details->presentModeCount != 0)
                free(details->presentModes);
}
