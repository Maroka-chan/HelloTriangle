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
  createInfo->pNext = NULL;
  createInfo->flags = 0;
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
  RequiredExtensions glfwExtensions;
  glfwExtensions.extensions = glfwGetRequiredInstanceExtensions(&glfwExtensions.extensionCount);

  // Enable extra extensions when validation layers are enabled
  if (enableValidationLayers) {
    const char* extraExtenstions[] = {
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    int extraExtensionsSize = arrayLength(extraExtenstions);
    RequiredExtensions extensions;
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
  VkApplicationInfo appInfo;

  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Hello Triangle";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "No Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;


  // Here we tell the Vulkan Driver which Global Extensions and Validation Layers we want to use. (Required)
  VkInstanceCreateInfo createInfo;

  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Get the extensions required to interface with the window system from GLFW.
  // We also get additional extensions when validation layers are enabled.
  RequiredExtensions requiredExtensions = getRequiredExtensions();

  createInfo.enabledExtensionCount = requiredExtensions.extensionCount;
  createInfo.ppEnabledExtensionNames = requiredExtensions.extensions;

  // Add validation layers if enabled
  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
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
    abort();
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
    abort();
  }
}

void setupDebugMessenger() {
  if (!enableValidationLayers) return;
  
  VkDebugUtilsMessengerCreateInfoEXT createInfo;
  populateDebugMessengerCreateInfo(&createInfo);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
    error("Failed to set up debug messenger!\n");
    abort();
  }
}

typedef struct QueueFamilyIndices {
  option(uint32_t) graphicsFamily;
} QueueFamilyIndices;

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

  VkQueueFamilyProperties queueFamilies[queueFamilyCount];
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

  int i = 0;
  foreach(queueFamily, queueFamilies) {
    if (queueFamily->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      setOption(indices.graphicsFamily, i);
    }
    if (indices.graphicsFamily.isSome) break;

    i++;
  }

  return indices;
}

bool isDeviceSuitable(VkPhysicalDevice device) {
  QueueFamilyIndices indices = findQueueFamilies(device);

  return indices.graphicsFamily.isSome;
}

void pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

  if (deviceCount == 0) {
    error("Failed to find GPUs with Vulkan support!\n");
    abort();
  }

  VkPhysicalDevice devices[deviceCount];
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

  foreach(device, devices) {
    if (isDeviceSuitable(*device)) {
      physicalDevice = *device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    error("Failed to find a suitable GPU!\n");
    abort();
  }
}

void initVulkan() {
  createInstance();
  setupDebugMessenger();
  pickPhysicalDevice();
}

void mainLoop() {
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void cleanup() {
  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);
  }

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
