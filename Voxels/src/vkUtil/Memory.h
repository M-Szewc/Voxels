#pragma once
#include "Base.h"

namespace vkUtil {

	struct BufferInput {
		size_t size;
		vk::BufferUsageFlags usage;
		vk::Device logicalDevice;
		vk::PhysicalDevice physicalDevice;
		vk::MemoryPropertyFlags memoryProperties;
	};

	struct Buffer {
		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
	};

	Buffer CreateBuffer(BufferInput input);

	uint32_t FindMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);

	void AllocateBufferMemory(Buffer& buffer, const BufferInput& input);

	void CopyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer);
}