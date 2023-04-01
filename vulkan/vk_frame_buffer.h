#ifndef VK_FRAME_BUFFER_H
#define VK_FRAME_BUFFER_H

#include "vk_swap_chain.h"
#include <vulkan/vulkan_core.h>

VkResult create_frame_buffers(
                VkDevice p_device,
                struct SwapChainDetails *p_swap_chain_details,
                VkImageView *a_image_views,
                VkRenderPass *p_render_pass,
                VkFramebuffer **a_frame_buffers);

void destroy_frame_buffers(VkDevice *p_device,
                VkFramebuffer *a_frame_buffers,
                uint32_t buffer_count);

#endif
