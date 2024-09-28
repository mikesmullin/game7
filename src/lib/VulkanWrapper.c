#include "VulkanWrapper.h"

#include "Vulkan.h"

static Vulkan_t* s_Vulkan;

void VulkanWrapper__Init(void* Vulkan) {
  s_Vulkan = Vulkan;
}

void VulkanWrapper__UpdateTextureImage(const Bitmap_t* bmp) {
  Vulkan__UpdateTextureImage(s_Vulkan, bmp);
}

void VulkanWrapper__UpdateVertexBuffer(u8 idx, u64 size, const void* indata) {
  Vulkan__UpdateVertexBuffer(s_Vulkan, idx, size, indata);
}

void VulkanWrapper__UpdateUniformBuffer(u8 frame, void* ubo) {
  Vulkan__UpdateUniformBuffer(s_Vulkan, frame, ubo);
}

void VulkanWrapper__SetInstanceCount(u32 instanceCount) {
  s_Vulkan->m_instanceCount = instanceCount;
}

u8 VulkanWrapper__GetCurrentFrame() {
  return s_Vulkan->m_currentFrame;
}

void VulkanWrapper__SetAspectRatio(f32 aspectRatio) {
  s_Vulkan->m_aspectRatio = aspectRatio;
}
