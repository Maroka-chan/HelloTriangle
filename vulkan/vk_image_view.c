#include <vulkan/vulkan_core.h>
#include <stdlib.h>


VkResult create_image_views(VkDevice *p_device,
                VkImage *a_images,
                uint32_t image_count,
                VkFormat *p_image_format,
                VkImageView *a_image_views)
{
        a_image_views =
                malloc(image_count * sizeof(VkImageView));

        for (size_t i = 0; i < image_count; i++) {
                VkImageViewCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                createInfo.image = a_images[i];
                createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                createInfo.format = *p_image_format;

                createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                createInfo.subresourceRange.aspectMask =
                        VK_IMAGE_ASPECT_COLOR_BIT;
                createInfo.subresourceRange.baseMipLevel = 0;
                createInfo.subresourceRange.levelCount = 1;
                createInfo.subresourceRange.baseArrayLayer = 0;
                createInfo.subresourceRange.layerCount = 1;

                VkResult result = vkCreateImageView(*p_device, &createInfo, NULL,
                                &a_image_views[i]);
                if (result != VK_SUCCESS) {
                        return result;
                }
        }
        return VK_SUCCESS;
}
