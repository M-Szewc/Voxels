#pragma once

#include "Core/Base.h"
#include "vkUtil/VulkanUtil.h"

namespace vkInit{

	struct FramebufferInput {
		vk::Device Device;
		vk::RenderPass RenderPass;
		vk::Extent2D SwapchainExtent;
	};

	void CreateFramebuffers(FramebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames);

}