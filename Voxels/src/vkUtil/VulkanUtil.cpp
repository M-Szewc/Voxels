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
				indices.m_GraphicsFamily = i;

#ifdef VO_DEBUG
				VO_CORE_INFO("Queue Family {0} is suitable for graphics", i);
#endif
			}

			if (device.getSurfaceSupportKHR(i, surface)) {
				indices.m_PresentFamily = i;

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
		input.LogicalDevice = logicalDevice;
		input.PhysicalDevice = physicalDevice;
		input.MemoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		input.Size = sizeof(UniformBufferObject);
		input.Usage = vk::BufferUsageFlagBits::eUniformBuffer;
		m_CameraDataBuffer = CreateBuffer(input);

		m_CameraDataWriteLocation = logicalDevice.mapMemory(m_CameraDataBuffer.m_BufferMemory, 0, sizeof(UniformBufferObject));
	
		input.Size = 1024 * sizeof(glm::mat4);
		input.Usage = vk::BufferUsageFlagBits::eStorageBuffer;
		m_ModelBuffer = CreateBuffer(input);

		m_ModelBufferWriteLocation = logicalDevice.mapMemory(m_ModelBuffer.m_BufferMemory, 0, 1024 * sizeof(glm::mat4));

		m_ModelTransforms.reserve(1024);
		for (int i = 0; i < 1024; ++i) {
			m_ModelTransforms.push_back(glm::mat4(1.0f));
		}

		m_UniformBufferDescriptor.buffer = m_CameraDataBuffer.m_Buffer;
		m_UniformBufferDescriptor.offset = 0;
		m_UniformBufferDescriptor.range = sizeof(UniformBufferObject);
	
		m_ModelBufferDescriptor.buffer = m_ModelBuffer.m_Buffer;
		m_ModelBufferDescriptor.offset = 0;
		m_ModelBufferDescriptor.range = 1024 * sizeof(glm::mat4);

	}
}