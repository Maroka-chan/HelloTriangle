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
        VkSwapchainKHR swap_chain;  // Handle to the swap chain
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

void recreate_swap_chain(
                GLFWwindow *p_window,
                VkDevice device,
                VkImage *a_images,
                uint32_t image_count,
                VkFormat *p_image_format,
                VkImageView **a_image_views,
                VkPhysicalDevice physical_device,
                VkSurfaceKHR surface,
                struct SwapChainDetails *p_swap_chain_details,
                VkRenderPass *p_render_pass,
                VkFramebuffer **a_frame_buffers);

VkResult create_swap_chain(
                GLFWwindow *p_window,
                VkDevice device,
                VkPhysicalDevice physical_device,
                VkSurfaceKHR surface,
                struct SwapChainDetails *p_swap_chain_details);

struct SwapChainSupportDetails query_swap_chain_support(
                VkPhysicalDevice device, VkSurfaceKHR surface);

void cleanup_swap_chain(
                VkDevice device,
                VkSwapchainKHR swap_chain,
                VkFramebuffer *swap_chain_framebuffers,
                VkImageView *swap_chain_image_views,
                uint32_t swap_chain_framebuffer_count,
                uint32_t swap_chain_image_views_count
                );

void destroySwapChainSupportDetails(
                struct SwapChainSupportDetails *p_support_details);

#endif
