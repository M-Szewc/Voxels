#pragma once
#include "Core/Base.h"
#include "vkUtil/VulkanUtil.h"

namespace vkInit {

	struct CommandBufferInputChunk {
		vk::Device Device;
		vk::CommandPool CommandPool;
		std::vector<vkUtil::SwapChainFrame>& Frames;
	};

	vk::CommandPool CreateCommandPool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);

	vk::CommandBuffer CreateCommandBuffer(CommandBufferInputChunk inputChunk);

	void CreateFrameCommandBuffers(CommandBufferInputChunk inputChunk);
}