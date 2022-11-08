#include <stddef.h>
#include <stdint.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "debug/print.h"
#include "option.h"
#include "vulkan/vk_ValidationLayers.h"
#include "vulkan/vk_DebugUtilsMessenger.h"
#include "vulkan/vk_QueueFamilies.h"
#include "vulkan/vk_SwapChain.h"


#define arrayLength(arr) \
  *(&arr + 1) - arr

#define foreach(item, list) \
  for(typeof(list[0]) *item = list; item < (&list)[1]; item++)

// Enable Validation Layers only in Debug Mode
#ifdef DEBUG
  const bool enableValidationLayers = true;
#else
  const bool enableValidationLayers = false;
#endif

// Window Size
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* window;

// Validation Layers to request/enable
const char *validationLayers[] = {
  "VK_LAYER_KHRONOS_validation"
};
const uint32_t validationLayerCount = sizeof(validationLayers) / sizeof(validationLayers[0]);

// Device Extensions to enable
const char *deviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


// Handle to the Vulkan library instance
VkInstance instance;

// Handle to the physical device(gpu) to use
// This object will be implicitly destroyed when the VkInstance is destroyed
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

// Handle to the logical device used for interfacing the physical device
VkDevice device;

// Handle to the graphics queue
// Device queues are implicitly cleaned up when
// the device is destroyed
VkQueue graphicsQueue;
// Handle to the present queue
VkQueue presentQueue;


SwapChainDetails swapChainDetails;


// Handle to the swap chain image views
VkImageView* swapChainImageViews;


// Handle to the Debug callback
VkDebugUtilsMessengerEXT debugMessenger;

// Handle to the window surface
VkSurfaceKHR surface;


void initWindow() {
  glfwInit();

  // Tell glfw to not use OpenGL since we use Vulkan
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // Disable the ability to resize the window
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

typedef struct RequiredExtensions {
  const char** extensions;
  uint32_t extensionCount;
} RequiredExtensions;

const RequiredExtensions getRequiredExtensions() {
  RequiredExtensions glfwExtensions = {};
  glfwExtensions.extensions = glfwGetRequiredInstanceExtensions(&glfwExtensions.extensionCount);

  // Enable extra extensions when validation layers are enabled
  if (enableValidationLayers) {
    const char* extraExtenstions[] = {
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    size_t extraExtensionsSize = arrayLength(extraExtenstions);
    RequiredExtensions extensions = {};
    extensions.extensionCount = glfwExtensions.extensionCount + extraExtensionsSize;
    extensions.extensions = malloc(extensions.extensionCount);
   
    // Merge the arrays
    size_t i,j;
    for (i = 0; i < glfwExtensions.extensionCount; i++) extensions.extensions[i] = glfwExtensions.extensions[i];
    for (i = 0, j = glfwExtensions.extensionCount; j < extensions.extensionCount && i < extraExtensionsSize; i++, j++) extensions.extensions[j] = extraExtenstions[i];
  
    return extensions;
  }

  return glfwExtensions;
}

void createInstance() {
  // We specify some information about our application so that it can be used for optimizations. (Optional)
  VkApplicationInfo appInfo = {};

  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;


  // Here we tell the Vulkan Driver which Global Extensions and Validation Layers we want to use. (Required)
  VkInstanceCreateInfo createInfo = {};

  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Get the extensions required to interface with the window system from GLFW.
  // We also get additional extensions when validation layers are enabled.
  RequiredExtensions requiredExtensions = getRequiredExtensions();

  createInfo.enabledExtensionCount = requiredExtensions.extensionCount;
  createInfo.ppEnabledExtensionNames = requiredExtensions.extensions;

  // Add validation layers if enabled
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = arrayLength(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    // Add debug messenger to be used during vkCreateInstance and vkDestroyInstance
    // The debug messenger will automatically be cleaned up afterwards
    populateDebugMessengerCreateInfo(&debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;

  } else {
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = NULL;
  }

  // Create the Vulkan instance
  VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
  if (result != VK_SUCCESS) {
    error("Failed to create Vulkan instance!\n");
    exit(EXIT_FAILURE);
  }

  // Get the number of available extensions
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

  // Get the details of the extensions
  VkExtensionProperties extensions[extensionCount];
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

  // Print the available extensions
  printf("Available Extensions:\n");

  for (size_t i = 0; i < extensionCount; i++) {
    printf("\t %s\n", extensions[i].extensionName);
  }

  // Check that the requested validation layers are available
  if (enableValidationLayers && !checkValidationLayerSupport(validationLayers, validationLayerCount)) {
    error("Validation layers requested, but not available!\n");
    exit(EXIT_FAILURE);
  }
}









bool checkDeviceExtensionSupport(VkPhysicalDevice* device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, NULL);
  
  VkExtensionProperties availableExtensions[extensionCount];
  vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, availableExtensions);

  // TODO Temporary solution since we do not have a Set data structure
  size_t requiredExtensionCount = arrayLength(deviceExtensions);
  uint32_t requiredExtensionsFound = 0;
  foreach(requiredExtension, deviceExtensions) {
    if (requiredExtensionsFound == requiredExtensionCount) break;
    foreach(extension, availableExtensions) {
      if (strcmp(extension->extensionName, *requiredExtension)) {
        requiredExtensionsFound++;
        break;
      }
    }
  }

  return requiredExtensionsFound == requiredExtensionCount;
}

bool isDeviceSuitable(VkPhysicalDevice* device) {
  QueueFamilyIndices indices = findQueueFamilies(*device, surface);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(*device, surface);
    swapChainAdequate = //TODO
      swapChainSupport.surfaceFormatCount != 0 &&
      swapChainSupport.presentModeCount != 0;
  }

  return 
    isQueueFamilyIndicesComplete(&indices) &&
    extensionsSupported &&
    swapChainAdequate;
}

void pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

  if (deviceCount == 0) {
    error("Failed to find GPUs with Vulkan support!\n");
    exit(EXIT_FAILURE);
  }

  VkPhysicalDevice devices[deviceCount];
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

  for (size_t i = 0; i < deviceCount; i++) {
    VkPhysicalDevice device = devices[i]; 
    if (isDeviceSuitable(&device)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    error("Failed to find a suitable GPU!\n");
    exit(EXIT_FAILURE);
  }
}

void createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

  uint32_t queueCount = 0;
  // TODO Temporary solution to avoid duplicates since we do not have a Set data structure
  if (indices.graphicsFamily.value == indices.presentFamily.value)
    queueCount = 1;
  else queueCount = 2;
  // TODO Implement List data structure
  VkDeviceQueueCreateInfo queueCreateInfos[queueCount];
  // TODO Implement Set data structure
  // The index to the queue families are supposed to only be added once if they are the same
  // , but they will be added multiple times since we are not using a Set.
  uint32_t uniqueQueueFamilies[queueCount];
  uniqueQueueFamilies[0] = indices.graphicsFamily.value;
  // TODO Temporary solution to avoid duplicates since we do not have a Set data structure
  if (queueCount == 2) uniqueQueueFamilies[1] = indices.presentFamily.value;

  float queuePriority = 1.0f;
  for(int i = 0; i < queueCount; i++) {
    uint32_t queueFamily = uniqueQueueFamilies[i];
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos[i] = queueCreateInfo;
  }

  VkPhysicalDeviceFeatures deviceFeatures = {};

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos;
  createInfo.queueCreateInfoCount = queueCount;

  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount = arrayLength(deviceExtensions);
  createInfo.ppEnabledExtensionNames = deviceExtensions;

  // For compatibility with older versions of Vulkan
  // There is no longer any distinction between instance and 
  // device specific validation layers
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = arrayLength(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS) {
    error("Failed to create logical device!\n");
    exit(EXIT_FAILURE);
  }

  // We are only creating a single queue from each of these families, so we'll simply use the queue at index 0
  vkGetDeviceQueue(device, indices.graphicsFamily.value, 0, &graphicsQueue);
  vkGetDeviceQueue(device, indices.presentFamily.value, 0, &presentQueue);
}

void createSurface() {
  if (glfwCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
    error("Failed to create window surface!\n");
    exit(EXIT_FAILURE);
  }
}


void createImageViews(uint32_t swapChainImageCount, VkImage* swapChainImages, VkFormat swapChainImageFormat) {
  swapChainImageViews = malloc(swapChainImageCount * sizeof(VkImageView));

  for (size_t i = 0; i < swapChainImageCount; i++) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = swapChainImages[i];
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = swapChainImageFormat;

    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device, &createInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
      error("Failed to create image views!\n");
      exit(EXIT_FAILURE);
    }
  }

}

void initVulkan() {
  createInstance();
  if (enableValidationLayers) {
    setupDebugMessenger(instance, &debugMessenger);
  }
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  swapChainDetails = createSwapChain(window, device, physicalDevice, surface);
  createImageViews(swapChainDetails.swapChainImageCount, swapChainDetails.swapChainImages, swapChainDetails.swapChainImageFormat);
}

void mainLoop() {
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void cleanup() {
  foreach(imageView, swapChainImageViews) {
    vkDestroyImageView(device, *imageView, NULL);
  }

  vkDestroySwapchainKHR(device, *swapChainDetails.swapChain, NULL);

  vkDestroyDevice(device, NULL);

  if (enableValidationLayers) {
    destroyDebugMessenger(instance, debugMessenger, NULL);
  }

  vkDestroySurfaceKHR(instance, surface, NULL);
  vkDestroyInstance(instance, NULL);

  glfwDestroyWindow(window);

  glfwTerminate();
}

void run() {
  initWindow();
  initVulkan();
  mainLoop();
  cleanup();
}

int main() {
  run();

  return EXIT_SUCCESS;
}
