#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "../debug/print.h"

VkCommandBuffer *create_command_buffer(
                VkDevice *p_device,
                VkCommandPool *p_command_pool,
                uint32_t max_frames_in_flight)
{
        VkCommandBuffer *commandBuffers = malloc(sizeof(VkCommandBuffer) * max_frames_in_flight);

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = *p_command_pool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = max_frames_in_flight;

        if (vkAllocateCommandBuffers(*p_device, &allocInfo, commandBuffers)
                        != VK_SUCCESS) {
                error("Failed to allocate command buffers!");
                exit(EXIT_FAILURE);
        }

        return commandBuffers;
}

void record_command_buffer(
                VkRenderPass *p_render_pass,
                VkFramebuffer *frame_buffers,
                VkExtent2D *p_extent,
                VkPipeline *p_graphics_pipeline,
                VkCommandBuffer command_buffer,
                uint32_t image_index)
{
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional
        beginInfo.pInheritanceInfo = NULL; // Optional
        
        if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS) {
                error("Failed to begin recording command buffer!");
                exit(EXIT_FAILURE);
        }

        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = *p_render_pass;
        renderPassInfo.framebuffer = frame_buffers[image_index];
        VkOffset2D offset = {0, 0};
        renderPassInfo.renderArea.offset = offset;
        renderPassInfo.renderArea.extent = *p_extent;
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(command_buffer, &renderPassInfo,
                        VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                        *p_graphics_pipeline);

        vkCmdDraw(command_buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(command_buffer);

        if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
                error("Failed to record command buffer!");
                exit(EXIT_FAILURE);
        }
}
