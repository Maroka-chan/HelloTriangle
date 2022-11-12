#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "../option.h"
#include "../debug/print.h"
#include "vk_queue_family.h"
#include "vk_swap_chain.h"


struct SwapChainSupportDetails query_swap_chain_support(
                VkPhysicalDevice device,
                VkSurfaceKHR surface)
{
        struct SwapChainSupportDetails supportDetails = {};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, 
                        &supportDetails.capabilities);

        uint32_t surfaceformat_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, 
                        &surfaceformat_count, NULL);

        supportDetails.surface_format_count = surfaceformat_count;
        if (surfaceformat_count != 0) {
                supportDetails.formats = 
                        malloc(surfaceformat_count * 
                                        sizeof(VkSurfaceFormatKHR));
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, 
                                &surfaceformat_count, supportDetails.formats);
        }

        uint32_t presentmode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
                        &presentmode_count, NULL);

        supportDetails.present_mode_count = presentmode_count;
        if (presentmode_count != 0) {
                supportDetails.present_modes = 
                        malloc(presentmode_count * sizeof(VkPresentModeKHR));
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
                                &presentmode_count, supportDetails.present_modes);
        }

        return supportDetails;
}


// Color Depth
// Chooses the color format and colorspace
static VkSurfaceFormatKHR choose_surfaceformat(
                const VkSurfaceFormatKHR *available_formats,
                uint32_t format_count)
{
        for (size_t i = 0; i < format_count; i++) {
                if (available_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && 
                                available_formats[i].colorSpace 
                                == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                        return available_formats[i];
                }
        }

        return available_formats[0];
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
static VkPresentModeKHR choose_present_mode(
                const VkPresentModeKHR *available_present_modes, 
                uint32_t present_mode_count)
{
        for (size_t i = 0; i < present_mode_count; i++) {
                if (available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        return available_present_modes[i];
                }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
}


static double clamp(double d, double min, double max)
{
        const double t = d < min ? min : d;
        return t > max ? max : t;
}

static VkExtent2D choose_swap_extent(GLFWwindow *p_window,
                VkSurfaceCapabilitiesKHR capabilities)
{
        if (capabilities.currentExtent.width != UINT32_MAX) {
                return capabilities.currentExtent;
        } else {
                int width, height;
                glfwGetFramebufferSize(p_window, &width, &height);

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

static VkSwapchainCreateInfoKHR create_swap_chain_info(
                VkPhysicalDevice physical_device,
                VkSurfaceKHR surface,
                VkSurfaceFormatKHR surface_format,
                uint32_t image_count,
                VkExtent2D extent,
                struct SwapChainSupportDetails support_details,
                VkPresentModeKHR present_mode)
{
        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = image_count;
        createInfo.imageFormat = surface_format.format;
        createInfo.imageColorSpace = surface_format.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;

        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        struct QueueFamilyIndices indices =
                find_queue_families(physical_device, surface);

        uint32_t queueFamilyIndices[] = { indices.graphics_family.value,
                indices.present_family.value };

        if (indices.graphics_family.value != indices.present_family.value) {
                createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                createInfo.queueFamilyIndexCount = 2;
                createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
                createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                createInfo.queueFamilyIndexCount = 0; // Optional
                createInfo.pQueueFamilyIndices = NULL; // Optional
        }

        createInfo.preTransform =
                support_details.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = present_mode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        return createInfo;
}

VkResult create_swap_chain(
                GLFWwindow *p_window,
                VkDevice device,
                VkPhysicalDevice physical_device,
                VkSurfaceKHR surface,
                struct SwapChainDetails *p_swap_chain_details)
{
        struct SwapChainSupportDetails supportDetails =
                query_swap_chain_support(physical_device, surface);

        VkSurfaceFormatKHR surfaceFormat =
                choose_surfaceformat(supportDetails.formats,
                                supportDetails.surface_format_count);

        VkPresentModeKHR presentMode =
                choose_present_mode(supportDetails.present_modes,
                                supportDetails.present_mode_count);

        VkExtent2D swapExtent = choose_swap_extent(p_window,
                        supportDetails.capabilities);

        // We should request at least one more than the minimum to avoid
        // having to wait for the driver to complete internal operations
        // before we can acquire another image to render to.
        uint32_t image_count =
                supportDetails.capabilities.minImageCount + 1;

        // Make sure we do not exceed the maximum image count.
        // We first check that the maximum is larger than 0 since
        // 0 is a special value that means that there is no maximum.
        if (supportDetails.capabilities.maxImageCount > 0 &&
                        image_count >
                        supportDetails.capabilities.maxImageCount) {

                image_count =
                        supportDetails.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo =
                create_swap_chain_info(physical_device, surface,
                                surfaceFormat, image_count,
                                swapExtent, supportDetails, presentMode);

        VkSwapchainKHR p_swap_chain;
        VkResult result = vkCreateSwapchainKHR(device, &createInfo,
                        NULL, &p_swap_chain);
        if (result != VK_SUCCESS) {
                return result;              
        }

        vkGetSwapchainImagesKHR(device, p_swap_chain,
                        &image_count, NULL);

        VkImage *images =
                malloc(image_count * sizeof(VkImage));
        vkGetSwapchainImagesKHR(device, p_swap_chain,
                        &image_count, images);

        VkFormat image_format = surfaceFormat.format;

        p_swap_chain_details->p_swap_chain = &p_swap_chain;
        p_swap_chain_details->extent = swapExtent;
        p_swap_chain_details->image_format = image_format;
        p_swap_chain_details->images = images;
        p_swap_chain_details->image_count = image_count;

        return VK_SUCCESS;
}

// TODO Call somewhere. Destroy on cleanup()?
void destroySwapChainSupportDetails(
                struct SwapChainSupportDetails *p_support_details)
{
        if (p_support_details->surface_format_count != 0)
                free(p_support_details->formats);
        if (p_support_details->present_mode_count != 0)
                free(p_support_details->present_modes);
}
