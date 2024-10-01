#pragma once

#include "Base.h"
#include "vkUtil/VulkanUtil.h"

namespace vkInit{

	struct FramebufferInput {
		vk::Device device;
		vk::RenderPass renderPass;
		vk::Extent2D swapchainExtent;
	};

	void CreateFramebuffers(FramebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames);

}