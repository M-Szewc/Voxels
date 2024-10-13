#pragma once
#include "Core/Base.h"

#include "vkUtil/Memory.h"

namespace vkUtil {

	struct QueueFamilyIndices {
		std::optional<uint32_t> m_GraphicsFamily;
		std::optional<uint32_t> m_PresentFamily;

		bool IsComplete() {
			return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
		}
	};


	// returns queue family indices
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	struct UniformBufferObject {
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 viewProjection;
	};
	
	struct SwapChainFrame {

		//swapchain
		vk::Image m_Image;
		vk::ImageView m_ImageView;
		vk::Framebuffer m_FrameBuffer;
		
		vk::CommandBuffer m_CommandBuffer;
		
		//synchronization
		vk::Semaphore m_ImageAveilable, m_RenderFinished;
		vk::Fence m_InFlightFence;
		
		//resources
		UniformBufferObject m_CameraData;
		Buffer m_CameraDataBuffer;
		void* m_CameraDataWriteLocation;

		std::vector<glm::mat4> m_ModelTransforms;
		Buffer m_ModelBuffer;
		void* m_ModelBufferWriteLocation;

		//resource descriptors
		vk::DescriptorBufferInfo m_UniformBufferDescriptor;
		vk::DescriptorBufferInfo m_ModelBufferDescriptor;
		vk::DescriptorSet m_DescriptorSet;

		void CreateDescriptorResources(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);

		void WriteDescriptorSet(vk::Device device) {
			
			vk::WriteDescriptorSet writeInfo;

			writeInfo.dstSet = m_DescriptorSet;
			writeInfo.dstBinding = 0;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
			writeInfo.pBufferInfo = &m_UniformBufferDescriptor;

			device.updateDescriptorSets(writeInfo, nullptr);
			
			vk::WriteDescriptorSet writeInfo2;

			writeInfo2.dstSet = m_DescriptorSet;
			writeInfo2.dstBinding = 1;
			writeInfo2.dstArrayElement = 0;
			writeInfo2.descriptorCount = 1;
			writeInfo2.descriptorType = vk::DescriptorType::eStorageBuffer;
			writeInfo2.pBufferInfo = &m_ModelBufferDescriptor;

			device.updateDescriptorSets(writeInfo2, nullptr);

		}
	};


}