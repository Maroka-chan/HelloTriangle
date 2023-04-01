#ifndef VK_VERTEX_DATA_H
#define VK_VERTEX_DATA_H

#include <vulkan/vulkan_core.h>
#include <cglm/cglm.h>

typedef struct s_vertex {
    vec2 pos;
    vec3 color;
} Vertex;

VkVertexInputBindingDescription get_binding_description();

VkVertexInputAttributeDescription *get_attribute_description();

#endif