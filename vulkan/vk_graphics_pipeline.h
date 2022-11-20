#ifndef VK_GRAPHICS_PIPELINE_H
#define VK_GRAPHICS_PIPELINE_H

#include <vulkan/vulkan_core.h>

struct GraphicsPipelineDetails {
        VkPipeline graphics_pipeline;
        VkPipelineLayout pipeline_layout;
};

struct GraphicsPipelineDetails create_graphics_pipeline(
                VkDevice *p_device,
                VkExtent2D *p_swap_chain_extent,
                VkRenderPass *p_render_pass);

#endif
