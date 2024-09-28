#pragma once
#include "Base.h"
#include "VulkanUtil.h"

namespace vkInit {

	struct CommandBufferInputChunk {
		vk::Device device;
		vk::CommandPool commandPool;
		std::vector<vkUtil::SwapChainFrame>& frames;
	};

	vk::CommandPool CreateCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

	vk::CommandBuffer CreateCommandBuffer(CommandBufferInputChunk inputChunk);

	void CreateFrameCommandBuffers(CommandBufferInputChunk inputChunk);
}