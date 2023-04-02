#include <vulkan/vulkan_core.h>
#include <stddef.h>

#include "vk_vertex_data.h"


VkVertexInputBindingDescription get_binding_description() 
{
    VkVertexInputBindingDescription binding_description = {};
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Could be VK_VERTEX_INPUT_RATE_INSTANCE
    return binding_description;
}

VkVertexInputAttributeDescription *get_attribute_description() 
{
    VkVertexInputAttributeDescription *attribute_descriptions 
            = malloc(sizeof(VkVertexInputAttributeDescription) * 2);

    // Position attribute
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, pos);

    // Color attribute
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color);

    return attribute_descriptions;
}
