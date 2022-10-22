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

#define arrayLength(arr) \
  *(&arr + 1) - arr

#define foreach(item, list) \
  for(typeof(list[0]) *item = list; item < (&list)[1]; item++)


// Window Size
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* window;

// Validation Layers to request/enable
const char *validationLayers[] = {
  "VK_LAYER_KHRONOS_validation"
};

// Device Extensions to enable
const char *deviceExtensions[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


// Enable Validation Layers only in Debug Mode
#ifdef DEBUG
  const bool enableValidationLayers = true;
#else
  const bool enableValidationLayers = false;
#endif

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


// Handle to the Debug callback
VkDebugUtilsMessengerEXT debugMessenger;


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
  
  if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    error("Validation Layer: %s\n", pCallbackData->pMessage);
  else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    warning("Validation Layer: %s\n", pCallbackData->pMessage);
  else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    info("Validation Layer: %s\n", pCallbackData->pMessage);
  return VK_FALSE;
}


// Handle to the window surface
VkSurfaceKHR surface;


// Loads the vkCreateDebugUtilsMessengerEXT extension function
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
  VkResult (*functionPtr)(VkInstance,const VkDebugUtilsMessengerCreateInfoEXT*,const VkAllocationCallbacks*,VkDebugUtilsMessengerEXT*);
  functionPtr = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

  if (functionPtr != NULL) {
    return functionPtr(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

// Loads the vkDestroyDebugUtilsMessengerEXT extension cleanup function
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
  void (*functionPtr)(VkInstance,VkDebugUtilsMessengerEXT,const VkAllocationCallbacks*);
  functionPtr = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

  if (functionPtr != NULL) {
    functionPtr(instance, debugMessenger, pAllocator);
  }
}

// Function for populating the vkDebugMessengerCreateInfoEXT struct.
// vkCreateDebugUtilsMessengerEXT require a valid instance and
// vkDestroyDebugUtilsMessengerEXT must be called before the instance is destroyed.
// 
// So we have this function so we can reuse it in createInstance in
// order to create a seperate debug utils messenger specifically for these functions.
// 
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
  createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
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





void initWindow() {
  glfwInit();

  // Tell glfw to not use OpenGL since we use Vulkan
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  // Disable the ability to resize the window
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

bool checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, NULL);

  VkLayerProperties availableLayers[layerCount];
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

  foreach (layerName, validationLayers) {
    bool layerFound = false;

    foreach (layerProperties, availableLayers) {
      if (strcmp(*layerName, layerProperties->layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) return false;
  }

  return true;
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
    int extraExtensionsSize = arrayLength(extraExtenstions);
    RequiredExtensions extensions = {};
    extensions.extensionCount = glfwExtensions.extensionCount + extraExtensionsSize;
    extensions.extensions = malloc(extensions.extensionCount);
   
    // Merge the arrays
    int i,j;
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
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = arrayLength(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;

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

  for (int i = 0; i < extensionCount; i++) {
    printf("\t %s\n", extensions[i].extensionName);
  }

  // Check that the requested validation layers are available
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    error("Validation layers requested, but not available!\n");
    exit(EXIT_FAILURE);
  }
}

void setupDebugMessenger() {
  if (!enableValidationLayers) return;
  
  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  populateDebugMessengerCreateInfo(&createInfo);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
    error("Failed to set up debug messenger!\n");
    exit(EXIT_FAILURE);
  }
}

typedef struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR* formats;
  VkPresentModeKHR* presentModes;
} SwapChainSupportDetails;

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);

  if (formatCount != 0) {
    details.formats = malloc(formatCount * sizeof(typeof(*details.formats)));
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats);
  }

  return details;
}


VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR** availableFormats) {
  foreach(availableFormat, *availableFormats) {
    if (availableFormat->format == VK_FORMAT_B8G8R8A8_SRGB &&
        availableFormat->colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return *availableFormat;
    }
  }

  return *availableFormats[0];
}


typedef struct QueueFamilyIndices {
  Option(uint32_t) graphicsFamily;
  Option(uint32_t) presentFamily;
} QueueFamilyIndices;

bool isQueueFamilyIndicesComplete(QueueFamilyIndices* queuefamilyindices) {
  return queuefamilyindices->graphicsFamily.isSome &&
    queuefamilyindices->presentFamily.isSome;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices = {};

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

  VkQueueFamilyProperties queueFamilies[queueFamilyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

  // Find queue families that support the features we want.
  // The present and graphics family can be separate queues, but
  // are often the same. Logic can be added to prefer a physical device
  // that supports both in the same queue for improved performance.
  int i = 0;
  // TODO Implement Linked List data structure
  foreach(queueFamily, queueFamilies) {
    if (queueFamily->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      setOption(indices.graphicsFamily, i);
    }
    
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (presentSupport) {
      setOption(indices.presentFamily, i);
    }

    if (isQueueFamilyIndicesComplete(&indices)) break;

    i++;
  }

  return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice* device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, NULL);
  
  VkExtensionProperties availableExtensions[extensionCount];
  vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, availableExtensions);

  // TODO Temporary solution since we do not have a Set data structure
  uint32_t requiredExtensionCount = arrayLength(deviceExtensions);
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
  QueueFamilyIndices indices = findQueueFamilies(*device);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(*device);
    swapChainAdequate =
      arrayLength(swapChainSupport.formats) != 0 &&
      arrayLength(swapChainSupport.presentModes) != 0;
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

  foreach(device, devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = *device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    error("Failed to find a suitable GPU!\n");
    exit(EXIT_FAILURE);
  }
}

void createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

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

void initVulkan() {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
}

void mainLoop() {
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void cleanup() {
  vkDestroyDevice(device, NULL);

  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
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
