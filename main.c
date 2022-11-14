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
#include "vulkan/vk_validation_layer.h"
#include "vulkan/vk_debug_messenger.h"
#include "vulkan/vk_queue_family.h"
#include "vulkan/vk_swap_chain.h"
#include "vulkan/vk_image_view.h"
#include "vulkan/vk_logical_device.h"
#include "utils/array.h"


#define foreach(item, list) \
        for(typeof(list[0]) *item = list; item < (&list)[1]; item++)

// Enable Validation Layers only in Debug Mode
#ifdef DEBUG
const bool ENABLE_VALIDATION_LAYERS = true;
#else
const bool ENABLE_VALIDATION_LAYERS = false;
#endif

// Window Size
static const uint32_t WIDTH = 800;
static const uint32_t HEIGHT = 600;

static GLFWwindow *p_window;

// Validation Layers to request/enable
static const char *VALIDATION_LAYERS[] = {
        "VK_LAYER_KHRONOS_validation"
};

// Device Extensions to enable
static const char *DEVICE_EXTENSIONS[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


// Handle to the Vulkan library instance
static VkInstance instance;

// Handle to the physical device(gpu) to use
// This object will be implicitly destroyed when the VkInstance is destroyed
static VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

// Handle to the logical device used for interfacing the physical device
static VkDevice device;

// Handle to the graphics queue
// Device queues are implicitly cleaned up when
// the device is destroyed
static VkQueue graphicsQueue;
// Handle to the present queue
static VkQueue presentQueue;


static struct SwapChainDetails swapChainDetails;


// Handle to the swap chain image views
static VkImageView *swapChainImageViews;


// Handle to the Debug callback
static VkDebugUtilsMessengerEXT debugMessenger;

// Handle to the window surface
static VkSurfaceKHR surface;


void init_window()
{
        glfwInit();

        // Tell glfw to not use OpenGL since we use Vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Disable the ability to resize the window
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        p_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
}

struct RequiredExtensions {
        const char **extensions;
        uint32_t extension_count;
};

const struct RequiredExtensions get_required_extensions()
{
        struct RequiredExtensions glfwExtensions = {};
        glfwExtensions.extensions =
                glfwGetRequiredInstanceExtensions(
                                &glfwExtensions.extension_count);

        // Enable extra extensions when validation layers are enabled
        if (ENABLE_VALIDATION_LAYERS) {
                const char *extraExtenstions[] = {
                        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
                };
                size_t extraExtensionsSize = ARRAY_SIZE(extraExtenstions);
                struct RequiredExtensions extensions = {};
                extensions.extension_count =
                        glfwExtensions.extension_count + extraExtensionsSize;
                extensions.extensions = malloc(extensions.extension_count);

                // Merge the arrays
                size_t i,j;
                for (i = 0; i < glfwExtensions.extension_count; i++)
                        extensions.extensions[i] =
                                glfwExtensions.extensions[i];

                for (i = 0, j = glfwExtensions.extension_count;
                                j < extensions.extension_count &&
                                i < extraExtensionsSize; i++, j++)
                        extensions.extensions[j] = extraExtenstions[i];

                return extensions;
        }

        return glfwExtensions;
}

void create_instance()
{
        // We specify some information about our application so that
        // it can be used for optimizations. (Optional)
        VkApplicationInfo appInfo = {};

        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;


        // Here we tell the Vulkan Driver which Global Extensions and
        // Validation Layers we want to use. (Required)
        VkInstanceCreateInfo createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Get the extensions required to interface with the
        // window system from GLFW.
        // We also get additional extensions when
        // validation layers are enabled.
        struct RequiredExtensions requiredExtensions =
                get_required_extensions();

        createInfo.enabledExtensionCount = requiredExtensions.extension_count;
        createInfo.ppEnabledExtensionNames = requiredExtensions.extensions;

        // Add validation layers if enabled
        if (ENABLE_VALIDATION_LAYERS) {
                createInfo.enabledLayerCount = ARRAY_SIZE(VALIDATION_LAYERS);
                createInfo.ppEnabledLayerNames = VALIDATION_LAYERS;

                VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
                // Add debug messenger to be used during
                // vkCreateInstance and vkDestroyInstance
                // The debug messenger will automatically
                // be cleaned up afterwards
                populate_debug_messenger_createinfo(&debugCreateInfo);
                createInfo.pNext =
                        (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
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
        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);

        // Get the details of the extensions
        VkExtensionProperties extensions[extension_count];
        vkEnumerateInstanceExtensionProperties(NULL,
                        &extension_count, extensions);

        // Print the available extensions
        printf("Available Extensions:\n");

        for (size_t i = 0; i < extension_count; i++) {
                printf("\t %s\n", extensions[i].extensionName);
        }

        // Check that the requested validation layers are available
        if (ENABLE_VALIDATION_LAYERS &&
                        !check_validation_layer_support(VALIDATION_LAYERS,
                                ARRAY_SIZE(VALIDATION_LAYERS))) {
                error("Validation layers requested, but not available!\n");
                exit(EXIT_FAILURE);
        }
}

bool check_device_extension_support(VkPhysicalDevice *p_device)
{
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(*p_device, NULL,
                        &extensionCount, NULL);

        VkExtensionProperties availableExtensions[extensionCount];
        vkEnumerateDeviceExtensionProperties(*p_device, NULL,
                        &extensionCount, availableExtensions);

        // TODO Temporary solution since we do not have a Set data structure
        size_t requiredExtensionCount = ARRAY_SIZE(DEVICE_EXTENSIONS);
        uint32_t requiredExtensionsFound = 0;
        foreach(requiredExtension, DEVICE_EXTENSIONS) {
                if (requiredExtensionsFound == requiredExtensionCount) break;
                foreach(extension, availableExtensions) {
                        if (strcmp(extension->extensionName,
                                                *requiredExtension)) {
                                requiredExtensionsFound++;
                                break;
                        }
                }
        }

        return requiredExtensionsFound == requiredExtensionCount;
}

bool is_device_suitable(VkPhysicalDevice *p_device)
{
        struct QueueFamilyIndices indices =
                find_queue_families(*p_device, surface);

        bool extensionsSupported = check_device_extension_support(p_device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
                struct SwapChainSupportDetails swapChainSupport =
                        query_swap_chain_support(*p_device, surface);
                swapChainAdequate = //TODO
                        swapChainSupport.surface_format_count != 0 &&
                        swapChainSupport.present_mode_count != 0;
        }

        return 
                is_queue_family_indices_complete(&indices) &&
                extensionsSupported &&
                swapChainAdequate;
}

void pick_physical_device()
{
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
                if (is_device_suitable(&device)) {
                        physicalDevice = device;
                        break;
                }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
                error("Failed to find a suitable GPU!\n");
                exit(EXIT_FAILURE);
        }
}

void create_surface()
{
        if (glfwCreateWindowSurface(instance, p_window, NULL, &surface)
                        != VK_SUCCESS) {
                error("Failed to create window surface!\n");
                exit(EXIT_FAILURE);
        }
}

static void init_vulkan()
{
        create_instance();
        if (ENABLE_VALIDATION_LAYERS) {
                setup_debug_messenger(instance, &debugMessenger);
        }
        create_surface();
        pick_physical_device();
        if (create_logical_device(&physicalDevice, &surface,
                                DEVICE_EXTENSIONS,
                                ARRAY_SIZE(DEVICE_EXTENSIONS),
                                VALIDATION_LAYERS,
                                ARRAY_SIZE(VALIDATION_LAYERS),
                                &device)
                        != VK_SUCCESS) {
                error("Failed to create logical device!\n");
                exit(EXIT_FAILURE);
        }

        struct QueueFamilyIndices queueFamilyIndices =
                find_queue_families(physicalDevice, surface);

        create_queue(&device,queueFamilyIndices.graphics_family.value,
                        &graphicsQueue);
        create_queue(&device,queueFamilyIndices.present_family.value,
                        &presentQueue);
        
        if(create_swap_chain(p_window, device, physicalDevice,
                                surface, &swapChainDetails)
                        != VK_SUCCESS) {
                error("Failed to create swap chain!\n");
                exit(EXIT_FAILURE);
        }

        if (create_image_views(&device,
                                swapChainDetails.images,
                                swapChainDetails.image_count,
                                &swapChainDetails.image_format,
                                swapChainImageViews)
                        != VK_SUCCESS) {
                error("Failed to create image views!\n");
                exit(EXIT_FAILURE);
        }
}

static void main_loop()
{
        while(!glfwWindowShouldClose(p_window)) {
                glfwPollEvents();
        }
}

static void cleanup()
{
        foreach(imageView, swapChainImageViews) {
                vkDestroyImageView(device, *imageView, NULL);
        }

        vkDestroySwapchainKHR(device, *swapChainDetails.p_swap_chain, NULL);

        vkDestroyDevice(device, NULL);

        if (ENABLE_VALIDATION_LAYERS) {
                destroy_debug_messenger(instance, debugMessenger, NULL);
        }

        vkDestroySurfaceKHR(instance, surface, NULL);
        vkDestroyInstance(instance, NULL);

        glfwDestroyWindow(p_window);

        glfwTerminate();
}

static void run()
{
        init_window();
        init_vulkan();
        main_loop();
        cleanup();
}

int main()
{
        run();
        return EXIT_SUCCESS;
}
