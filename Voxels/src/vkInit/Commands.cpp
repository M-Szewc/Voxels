#include "vopch.h"

#include "Commands.h"


namespace vkInit {

	vk::CommandPool CreateCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices queueFamilyIndices = vkUtil::FindQueueFamilies(physicalDevice, surface);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndices.m_GraphicsFamily.value();

		try {
			return device.createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create Command Pool");
#endif
		}
	}

	vk::CommandBuffer CreateCommandBuffer(CommandBufferInputChunk inputChunk) {

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.CommandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		try {
			vk::CommandBuffer commandBuffer = inputChunk.Device.allocateCommandBuffers(allocInfo)[0];

#ifdef VO_DEBUG
			VO_CORE_TRACE("Allocated main command buffer");
#endif
			return commandBuffer;
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to allocate main command buffer");
#endif
			return nullptr;
		}
	}

	void CreateFrameCommandBuffers(CommandBufferInputChunk inputChunk) {

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.CommandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		//Make a command buffer for each frame
		for (int i = 0; i < inputChunk.Frames.size(); i++) {
			try {
				inputChunk.Frames[i].m_CommandBuffer = inputChunk.Device.allocateCommandBuffers(allocInfo)[0];

#ifdef VO_DEBUG
				VO_CORE_TRACE("Allocated command buffer for frame {0}", i);
#endif
			}
			catch (vk::SystemError err) {
#ifdef VO_DEBUG
				VO_CORE_ERROR("Failed to allocate command buffer for frame {0}", i);
#endif
			}
		}
	}
}