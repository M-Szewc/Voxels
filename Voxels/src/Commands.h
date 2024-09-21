#pragma once
#include "vopch.h"
#include "VulkanUtil.h"

namespace vkInit {

	struct CommandBufferInputChunk {
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapChainFrame>& frames;
	};

	vk::CommandPool CreateCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {

		vkUtil::QueueFamilyIndices queueFamilyIndices = vkUtil::FindQueueFamilies(physicalDevice, surface);

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		try {
			return device.createCommandPool(poolInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create Command Pool");
#endif
		}
	}

	vk::CommandBuffer CreateCommandBuffers(CommandBufferInputChunk inputChunk) {

		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputChunk.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		for (int i = 0; i < inputChunk.frames.size(); ++i) {
			try {
				inputChunk.frames[i].CommandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

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

		try {
			vk::CommandBuffer commandBuffer = inputChunk.device.allocateCommandBuffers(allocInfo)[0];

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
}