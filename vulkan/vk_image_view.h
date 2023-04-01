#ifndef VK_IMAGE_VIEW_H
#define VK_IMAGE_VIEW_H

#include <vulkan/vulkan_core.h>

VkResult create_image_views(
                VkDevice p_device,
                VkImage *a_images,
                uint32_t image_count,
                VkFormat *p_image_format,
                VkImageView **a_image_views);

void destroy_image_views(VkDevice *p_device,
                VkImageView *a_image_views,
                uint32_t image_view_count);

#endif
