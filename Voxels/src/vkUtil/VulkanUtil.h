#pragma once
#include "Base.h"

#include "vkUtil/Memory.h"

namespace vkUtil {

	struct QueueFamilyIndices {
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool IsComplete() {
			return GraphicsFamily.has_value() && PresentFamily.has_value();
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
		vk::Image Image;
		vk::ImageView ImageView;
		vk::Framebuffer FrameBuffer;
		
		vk::CommandBuffer CommandBuffer;
		
		//synchronization
		vk::Semaphore ImageAveilable, RenderFinished;
		vk::Fence InFlightFence;
		
		//resources
		UniformBufferObject CameraData;
		Buffer CameraDataBuffer;
		void* CameraDataWriteLocation;

		std::vector<glm::mat4> ModelTransforms;
		Buffer ModelBuffer;
		void* ModelBufferWriteLocation;

		//resource descriptors
		vk::DescriptorBufferInfo UniformBufferDescriptor;
		vk::DescriptorBufferInfo ModelBufferDescriptor;
		vk::DescriptorSet DescriptorSet;

		void CreateDescriptorResources(vk::Device logicalDevice, vk::PhysicalDevice physicalDevice);

		void WriteDescriptorSet(vk::Device device) {
			
			vk::WriteDescriptorSet writeInfo;

			writeInfo.dstSet = DescriptorSet;
			writeInfo.dstBinding = 0;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
			writeInfo.pBufferInfo = &UniformBufferDescriptor;

			device.updateDescriptorSets(writeInfo, nullptr);
			
			vk::WriteDescriptorSet writeInfo2;

			writeInfo2.dstSet = DescriptorSet;
			writeInfo2.dstBinding = 1;
			writeInfo2.dstArrayElement = 0;
			writeInfo2.descriptorCount = 1;
			writeInfo2.descriptorType = vk::DescriptorType::eStorageBuffer;
			writeInfo2.pBufferInfo = &ModelBufferDescriptor;

			device.updateDescriptorSets(writeInfo2, nullptr);

		}
	};


}