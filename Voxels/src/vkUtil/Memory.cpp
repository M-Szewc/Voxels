#include "vopch.h"
#include "Memory.h"

namespace vkUtil {

	Buffer CreateBuffer(BufferInput input) {

		vk::BufferCreateInfo bufferInfo;
		bufferInfo.flags = vk::BufferCreateFlags();
		bufferInfo.size = input.Size;
		bufferInfo.usage = input.Usage;
		// No concurent queues
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		Buffer buffer;
		buffer.m_Buffer = input.LogicalDevice.createBuffer(bufferInfo);

		AllocateBufferMemory(buffer, input);

		return buffer;
	}

	uint32_t FindMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties) {

		vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {

			bool supported{ static_cast<bool>(supportedMemoryIndices & (1 << i)) };

			bool sufficient{ (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties };

			if (supported && sufficient) {
				return i;
			}
		}
	}

	void AllocateBufferMemory(Buffer& buffer, const BufferInput& input) {

		vk::MemoryRequirements memoryRequirements = input.LogicalDevice.getBufferMemoryRequirements(buffer.m_Buffer);

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryTypeIndex(
			input.PhysicalDevice, memoryRequirements.memoryTypeBits,
			input.MemoryProperties
		);

		buffer.m_BufferMemory = input.LogicalDevice.allocateMemory(allocInfo);
		//hardcoded memory offset
		input.LogicalDevice.bindBufferMemory(buffer.m_Buffer, buffer.m_BufferMemory, 0);
	}

	void CopyBuffer(Buffer& srcBuffer, Buffer& dstBuffer, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer commandBuffer)
	{
		commandBuffer.reset();

		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
		commandBuffer.begin(beginInfo);

		vk::BufferCopy copyRegion;
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		commandBuffer.copyBuffer(srcBuffer.m_Buffer, dstBuffer.m_Buffer, 1, &copyRegion);

		commandBuffer.end();

		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		queue.submit(1, &submitInfo, nullptr);
		queue.waitIdle();
	}
}