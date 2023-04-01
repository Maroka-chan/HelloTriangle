#include "vk_swap_chain.h"
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>

#include "../debug/print.h"

VkResult create_frame_buffers(
                VkDevice p_device,
                struct SwapChainDetails *p_swap_chain_details,
                VkImageView *a_image_views,
                VkRenderPass *p_render_pass,
                VkFramebuffer **a_frame_buffers)
{
        *a_frame_buffers = malloc(p_swap_chain_details->image_count
                        * sizeof(VkFramebuffer));
        VkFramebuffer *a = *a_frame_buffers;

        for (size_t i = 0; i < p_swap_chain_details->image_count; i++) {
                VkImageView attachments[] = {
                        a_image_views[i]
                };

                VkFramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.sType =
                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.renderPass = *p_render_pass;
                framebufferInfo.attachmentCount = 1;
                framebufferInfo.pAttachments = attachments;
                framebufferInfo.width = p_swap_chain_details->extent.width;
                framebufferInfo.height = p_swap_chain_details->extent.height;
                framebufferInfo.layers = 1;

                VkResult result = vkCreateFramebuffer(p_device,
                                &framebufferInfo, NULL,
                                &a[i]);
                if (result != VK_SUCCESS)
                        return result;
        }

        return VK_SUCCESS;
}

void destroy_frame_buffers(VkDevice *p_device,
                VkFramebuffer *a_frame_buffers,
                uint32_t buffer_count)
{
        for (size_t i = 0; i < buffer_count; i++) {
                vkDestroyFramebuffer(*p_device, a_frame_buffers[i], NULL);
        }
        free(a_frame_buffers);
}
