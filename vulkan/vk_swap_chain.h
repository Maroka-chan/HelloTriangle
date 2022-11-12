#ifndef VK_SWAP_CHAIN_H
#define VK_SWAP_CHAIN_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        uint32_t surface_format_count;
        VkSurfaceFormatKHR *formats;
        uint32_t present_mode_count;
        VkPresentModeKHR *present_modes;
};

struct SwapChainDetails {
        VkSwapchainKHR *p_swap_chain;  // Handle to the swap chain
        uint32_t image_count;
        // Handle to the swap chain images
        // The images will be automatically cleaned up once
        // the swap chain has been destroyed
        VkImage *images;
        // Handle to the swap chain image format
        VkFormat image_format;
        // Handle to the swap chain extent
        VkExtent2D extent;
};

VkResult create_swap_chain(
                GLFWwindow *p_window,
                VkDevice device,
                VkPhysicalDevice physical_device,
                VkSurfaceKHR surface,
                struct SwapChainDetails *p_swap_chain_details);

struct SwapChainSupportDetails query_swap_chain_support(
                VkPhysicalDevice device, VkSurfaceKHR surface);

void destroySwapChainSupportDetails(
                struct SwapChainSupportDetails *p_support_details);

#endif
