#ifndef VK_RENDER_PASS_H
#define VK_RENDER_PASS_H

#include <vulkan/vulkan_core.h>

VkRenderPass create_render_pass(
                VkDevice *p_device,
                VkFormat *p_image_format);

#endif
