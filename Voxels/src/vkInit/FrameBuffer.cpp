#include "vopch.h"

#include "FrameBuffer.h"


namespace vkInit {

	void CreateFramebuffers(FramebufferInput inputChunk, std::vector<vkUtil::SwapChainFrame>& frames) {

		for (int i = 0; i < frames.size(); ++i) {
			std::vector<vk::ImageView> attachments = {
				frames[i].m_ImageView
			};

			vk::FramebufferCreateInfo framebufferinfo = {};
			framebufferinfo.flags = vk::FramebufferCreateFlags();
			framebufferinfo.renderPass = inputChunk.RenderPass;
			framebufferinfo.attachmentCount = attachments.size();
			framebufferinfo.pAttachments = attachments.data();
			framebufferinfo.width = inputChunk.SwapchainExtent.width;
			framebufferinfo.height = inputChunk.SwapchainExtent.height;
			framebufferinfo.layers = 1;

			try {
				frames[i].m_FrameBuffer = inputChunk.Device.createFramebuffer(framebufferinfo);

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