#include "vopch.h"
#include "Memory.h"

namespace vkUtil {

	Buffer CreateBuffer(BufferInput input) {

		vk::BufferCreateInfo bufferInfo;
		bufferInfo.flags = vk::BufferCreateFlags();
		bufferInfo.size = input.size;
		bufferInfo.usage = input.usage;
		// No concurent queues
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		Buffer buffer;
		buffer.buffer = input.logicalDevice.createBuffer(bufferInfo);

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

		vk::MemoryRequirements memoryRequirements = input.logicalDevice.getBufferMemoryRequirements(buffer.buffer);

		vk::MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryTypeIndex(
			input.physicalDevice, memoryRequirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		buffer.bufferMemory = input.logicalDevice.allocateMemory(allocInfo);
		//hardcoded memory offset
		input.logicalDevice.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0);
	}
}