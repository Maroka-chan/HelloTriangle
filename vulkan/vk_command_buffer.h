#ifndef VK_COMMAND_BUFFER_H
#define VK_COMMAND_BUFFER_H

#include <vulkan/vulkan_core.h>
#include "vk_vertex_data.h"

VkCommandBuffer *create_command_buffer(
                VkDevice *p_device,
                VkCommandPool *p_command_pool,
                uint32_t max_frames_in_flight);

void record_command_buffer(
                VkRenderPass *p_render_pass,
                VkFramebuffer *frame_buffers,
                VkExtent2D *p_extent,
                VkPipeline *p_graphics_pipeline,
                VkCommandBuffer command_buffer,
                uint32_t image_index,
                const Vertex *vertices,
                const uint32_t vertices_size,
                VkBuffer p_vertex_buffer);

#endif
