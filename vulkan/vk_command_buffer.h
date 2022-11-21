#ifndef VK_COMMAND_BUFFER_H
#define VK_COMMAND_BUFFER_H

#include <vulkan/vulkan_core.h>

VkCommandBuffer create_command_buffer(
                VkDevice *p_device,
                VkCommandPool *p_command_pool);

void record_command_buffer(
                VkRenderPass *p_render_pass,
                VkFramebuffer *frame_buffers,
                VkExtent2D *p_extent,
                VkPipeline *p_graphics_pipeline,
                VkCommandBuffer command_buffer,
                uint32_t image_index);

#endif
