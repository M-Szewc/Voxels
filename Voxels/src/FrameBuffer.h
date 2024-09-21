#pragma once

#include "vopch.h"
#include "VulkanUtil.h"

namespace vkInit{

	struct FramebufferInput {
		vk::Device device;
		vk::RenderPass renderPass;
		vk::Extent2D swapchainExtent;
	};

	void CreateFramebuffers(FramebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames) {
	
		for (int i = 0; i < frames.size(); ++i) {
			std::vector<vk::ImageView> attachments = {
				frames[i].ImageView
			};

			vk::FramebufferCreateInfo framebufferinfo = {};
			framebufferinfo.flags = vk::FramebufferCreateFlags();
			framebufferinfo.renderPass = inputChunk.renderPass;
			framebufferinfo.attachmentCount = attachments.size();
			framebufferinfo.pAttachments = attachments.data();
			framebufferinfo.width = inputChunk.swapchainExtent.width;
			framebufferinfo.height = inputChunk.swapchainExtent.height;
			framebufferinfo.layers = 1;

			try {
				frames[i].FrameBuffer = inputChunk.device.createFramebuffer(framebufferinfo);

#ifdef VO_DEBUG
				VO_CORE_TRACE("Created framebuffer for frame {0}", i);
#endif
			}
			catch (vk::SystemError err) {
#ifdef VO_DEBUG
				VO_CORE_ERROR("Failed to create framebuffer for frame {0}", i);
#endif
			}
		}
	}

}