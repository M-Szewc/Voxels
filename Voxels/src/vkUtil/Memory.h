#pragma once
#include "Core/Base.h"

namespace vkUtil {

	struct BufferInput {
		size_t Size;
		vk::BufferUsageFlags Usage;
		vk::Device LogicalDevice;
		vk::PhysicalDevice PhysicalDevice;
		vk::MemoryPropertyFlags MemoryProperties;
	};

	struct Buffer {
		vk::Buffer m_Buffer;
		vk::DeviceMemory m_BufferMemory;
	};

	Buffer CreateBuffer(BufferInput input);

	uint32_t FindMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties);

	void AllocateBufferMemory(Buffer& buffer, const BufferInput& input);

	void CopyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer);
}