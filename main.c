#include <stdint.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "debug.h"


// Window Size
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* window;

// Validation Layers to request/enable
const char *validationLayers[] = {
  "VK_LAYER_KHRONOS_validation"
};

// Enable Validation Layers only in Debug Mode
#ifdef NDEBUG
  const bool enableValidationLayers = false;
#else
  const bool enableValidationLayers = true;
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

  uint32_t enabledLayerCount = *(&validationLayers + 1) - validationLayers;
  for (int i = 0; i < enabledLayerCount; i++) {
    bool layerFound = false;

    for (int y = 0; y < layerCount;  y++) {
      if (strcmp(validationLayers[i], availableLayers[y].layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) return false;
  }

  return true;
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

  // Get the extensions needed to interface with the window system from GLFW.
  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;

  // Add validation layers if enabled
  if (enableValidationLayers) {
    createInfo.enabledLayerCount = *(&validationLayers + 1) - validationLayers;
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
