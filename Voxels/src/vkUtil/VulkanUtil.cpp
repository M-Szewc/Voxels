#include "vopch.h"

#include "VulkanUtil.h"

namespace vkUtil {

	// returns queue family indices
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
		QueueFamilyIndices indices;

		std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

#ifdef VO_DEBUG
		VO_CORE_INFO("System can support {0} queue families", queueFamilies.size());
#endif

		int i = 0;
		for (const vk::QueueFamilyProperties& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.GraphicsFamily = i;

#ifdef VO_DEBUG
				VO_CORE_INFO("Queue Family {0} is suitable for graphics", i);
#endif
			}

			if (device.getSurfaceSupportKHR(i, surface)) {
				indices.PresentFamily = i;

#ifdef VO_DEBUG
				VO_CORE_INFO("Queue Family {0} is suitable for presenting", i);
#endif
			}

			if (indices.IsComplete()) {
				break;
			}

			++i;
		}

		return indices;
	}

	void SwapChainFrame::CreateDescriptorResources(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice)
	{

		BufferInput input;
		input.logicalDevice = logicalDevice;
		input.physicalDevice = physicalDevice;
		input.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		input.size = sizeof(UniformBufferObject);
		input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
		CameraDataBuffer = CreateBuffer(input);

		CameraDataWriteLocation = logicalDevice.mapMemory(CameraDataBuffer.bufferMemory, 0, sizeof(UniformBufferObject));
	
		input.size = 1024 * sizeof(glm::mat4);
		input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
		ModelBuffer = CreateBuffer(input);

		ModelBufferWriteLocation = logicalDevice.mapMemory(ModelBuffer.bufferMemory, 0, 1024 * sizeof(glm::mat4));

		ModelTransforms.reserve(1024);
		for (int i = 0; i < 1024; ++i) {
			ModelTransforms.push_back(glm::mat4(1.0f));
		}

		UniformBufferDescriptor.buffer = CameraDataBuffer.buffer;
		UniformBufferDescriptor.offset = 0;
		UniformBufferDescriptor.range = sizeof(UniformBufferObject);
	
		ModelBufferDescriptor.buffer = ModelBuffer.buffer;
		ModelBufferDescriptor.offset = 0;
		ModelBufferDescriptor.range = 1024 * sizeof(glm::mat4);

	}
}