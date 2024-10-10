#pragma once

typedef struct Bitmap_t Bitmap_t;

#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;

void VulkanWrapper__Init(void* Vulkan);
void VulkanWrapper__UpdateTextureImage(const Bitmap_t* bmp);
void VulkanWrapper__UpdateVertexBuffer(u8 idx, u64 size, const void* indata);
void VulkanWrapper__UpdateUniformBuffer(u8 frame, void* ubo);
void VulkanWrapper__SetInstanceCount(u32 instanceCount);
u8 VulkanWrapper__GetCurrentFrame();
void VulkanWrapper__SetAspectRatio(f32 aspectRatio);
