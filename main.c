#include <stdint.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>


// Window Size
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

GLFWwindow* window;

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

  // Leave the global validation layers disabled for now
  createInfo.enabledLayerCount = 0;

  // Create the Vulkan instance
  VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
  if (result != VK_SUCCESS) {
    perror("Error: ");
    abort();
  }

  // Get the number of available extensions
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

  // Get the details of the extensions
  VkExtensionProperties extensions[extensionCount];
  vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensions);

  printf("Available Extensions:\n");

  for (int i = 0; i < extensionCount; i++) {
    printf("\t %s\n", extensions[i].extensionName);
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
