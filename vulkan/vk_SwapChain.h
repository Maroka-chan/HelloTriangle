#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

typedef struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  uint32_t surfaceFormatCount;
  VkSurfaceFormatKHR* formats;
  uint32_t presentModeCount;
  VkPresentModeKHR* presentModes;
} SwapChainSupportDetails;

typedef struct SwapChainDetails {
  VkSwapchainKHR* swapChain;  // Handle to the swap chain
  uint32_t swapChainImageCount;
  // Handle to the swap chain images
  // The images will be automatically cleaned up once
  // the swap chain has been destroyed
  VkImage* swapChainImages;
  // Handle to the swap chain image format
  VkFormat swapChainImageFormat;
  // Handle to the swap chain extent
  VkExtent2D swapChainExtent;
} SwapChainDetails;

SwapChainDetails createSwapChain(GLFWwindow* window, VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

#endif // !VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H
