#include "vulkan/vk_command_buffer.h"
#include "vulkan/vk_command_pool.h"
#include "vulkan/vk_frame_buffer.h"
#include "vulkan/vk_render_pass.h"
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
#include "vulkan/vk_instance_extension.h"
#include "vulkan/vk_physical_device.h"
#include "vulkan/vk_graphics_pipeline.h"
#include "vulkan/vk_swap_chain.h"
#include "vulkan/vk_vertex_data.h"

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


static const int MAX_FRAMES_IN_FLIGHT = 2;


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

static VkRenderPass renderPass;
static struct GraphicsPipelineDetails graphicsPipelineDetails;

static VkBuffer vertexBuffer;
static VkFramebuffer *swapChainFramebuffers;

static VkCommandPool commandPool;
static VkCommandBuffer *commandBuffers;


static VkSemaphore *imageAvailableSemaphores;
static VkSemaphore *renderFinishedSemaphores;
static VkFence *inFlightFences;

static uint32_t currentFrame = 0;

static bool frameBufferResized = false;

static const Vertex vertices[] = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};


static void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
        frameBufferResized = true;
}


void init_window()
{
        glfwInit();

        // Tell glfw to not use OpenGL since we use Vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        p_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);
        glfwSetFramebufferSizeCallback(p_window, framebuffer_resize_callback);
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

void create_surface()
{
        if (glfwCreateWindowSurface(instance, p_window, NULL, &surface)
                        != VK_SUCCESS) {
                error("Failed to create window surface!\n");
                exit(EXIT_FAILURE);
        }
}

static void create_sync_objects()
{
        imageAvailableSemaphores = malloc(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores = malloc(MAX_FRAMES_IN_FLIGHT);
        inFlightFences = malloc(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                if (vkCreateSemaphore(device, &semaphoreInfo, NULL,
                                        &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                                vkCreateSemaphore(device, &semaphoreInfo, NULL,
                                        &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                                vkCreateFence(device, &fenceInfo, NULL,
                                        &inFlightFences[i]) != VK_SUCCESS) {
                        error("Failed to create synchronization objects for a frame!");
                        exit(EXIT_FAILURE);
                }
        }
}

static void init_vulkan()
{
        create_instance();
        if (ENABLE_VALIDATION_LAYERS) {
                setup_debug_messenger(instance, &debugMessenger);
        }

        create_surface();

        pick_physical_device(
                        &instance,
                        &surface,
                        DEVICE_EXTENSIONS,
                        ARRAY_SIZE(DEVICE_EXTENSIONS),
                        &physicalDevice
                        );

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

        if (create_image_views(device,
                                swapChainDetails.images,
                                swapChainDetails.image_count,
                                &swapChainDetails.image_format,
                                &swapChainImageViews)
                        != VK_SUCCESS) {
                error("Failed to create image views!\n");
                exit(EXIT_FAILURE);
        }

        renderPass =
                create_render_pass(&device, &swapChainDetails.image_format);

        graphicsPipelineDetails = create_graphics_pipeline(
                        &device, &swapChainDetails.extent, &renderPass);

        if (create_frame_buffers(device,
                                &swapChainDetails,
                                swapChainImageViews,
                                &renderPass,
                                &swapChainFramebuffers) != VK_SUCCESS) {
                error("Failed to create framebuffer!");
                exit(EXIT_FAILURE);
        }

        commandPool = create_command_pool(&device, &physicalDevice, &surface);
        commandBuffers = create_command_buffer(&device, &commandPool, MAX_FRAMES_IN_FLIGHT);

        create_sync_objects();
}

void create_vertex_buffer()
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = sizeof(vertices);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, NULL, &vertexBuffer) != VK_SUCCESS) {
        printf("Failed to create vertex buffer!");
    }
}

void draw_frame()
{
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(device, swapChainDetails.swap_chain, UINT64_MAX,
                        imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                recreate_swap_chain(p_window, device,
                                &swapChainImageViews, physicalDevice, surface, 
                                &swapChainDetails, &renderPass, 
                                &swapChainFramebuffers);
                return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                error("Failed to acquire swap chain image!");
                exit(EXIT_FAILURE);
        }

        // Only reset the fence if we are submitting work
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        vkResetCommandBuffer(commandBuffers[currentFrame], 0);
        record_command_buffer(&renderPass, swapChainFramebuffers,
                        &swapChainDetails.extent,
                        &graphicsPipelineDetails.graphics_pipeline,
                        commandBuffers[currentFrame], imageIndex);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame])
                        != VK_SUCCESS) {
                error("Failed to submit draw command buffer!");
                exit(EXIT_FAILURE);
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapChainDetails.swap_chain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL;

        result = vkQueuePresentKHR(presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR
                        || frameBufferResized) {
                frameBufferResized = false;
                recreate_swap_chain(p_window, device,
                                &swapChainImageViews, physicalDevice, surface, 
                                &swapChainDetails, &renderPass, 
                                &swapChainFramebuffers);
        } else if (result != VK_SUCCESS) {
                error("Failed to present swap chain image!");
                exit(EXIT_FAILURE);
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

static void main_loop()
{
        while(!glfwWindowShouldClose(p_window)) {
                glfwPollEvents();
                draw_frame();
        }

        vkDeviceWaitIdle(device);
}

static void cleanup()
{
        cleanup_swap_chain(device, swapChainDetails.swap_chain,
                        swapChainFramebuffers, swapChainImageViews,
                        swapChainDetails.image_count,
                        swapChainDetails.image_count);
        vkDestroyBuffer(device, vertexBuffer, NULL);

        vkDestroyPipeline(device,
                        graphicsPipelineDetails.graphics_pipeline, NULL);

        vkDestroyPipelineLayout(device,
                        graphicsPipelineDetails.pipeline_layout, NULL);

        vkDestroyRenderPass(device, renderPass, NULL);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(device, renderFinishedSemaphores[i], NULL);
                vkDestroySemaphore(device, imageAvailableSemaphores[i], NULL);
                vkDestroyFence(device, inFlightFences[i], NULL);
        }

        vkDestroyCommandPool(device, commandPool, NULL);

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
