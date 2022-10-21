#include <stdint.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debug.h"

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

struct RequiredExtensions {
  const char** extensions;
  uint32_t extensionCount;
};

const struct RequiredExtensions getRequiredExtensions() {
  struct RequiredExtensions glfwExtensions;
  glfwExtensions.extensions = glfwGetRequiredInstanceExtensions(&glfwExtensions.extensionCount);

  // Enable extra extensions when validation layers are enabled
  if (enableValidationLayers) {
    const char* extraExtenstions[] = {
      VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };
    int extraExtensionsSize = arrayLength(extraExtenstions);
    struct RequiredExtensions extensions;
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
  struct RequiredExtensions requiredExtensions = getRequiredExtensions();

  createInfo.enabledExtensionCount = requiredExtensions.extensionCount;
  createInfo.ppEnabledExtensionNames = requiredExtensions.extensions;

  // Add validation layers if enabled
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = arrayLength(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;
  } else {
    createInfo.enabledLayerCount = 0;
  }

  // Create the Vulkan instance
  VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
  if (result != VK_SUCCESS) {
    error("Failed to create Vulkan instance\n");
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
    error("Validation layers requested, but not available!");
    abort();
  }
}

void initVulkan() {
  createInstance();
}

void mainLoop() {
  while(!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

void cleanup() {
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
