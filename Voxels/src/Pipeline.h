#pragma once
#include "Base.h"

namespace vkInit {

	struct GraphicsPipelineInBundle {
		vk::Device m_Device;
		std::string m_VertexFilepath;
		std::string m_FragmentFilepath;
		vk::Extent2D m_SwapchainExtent;
		vk::Format m_SwapchainImageFormat;
	};

	struct GraphicsPipelineOutBundle {
		vk::PipelineLayout m_PipelineLayout;
		vk::RenderPass m_RenderPass;
		vk::Pipeline m_Pipeline;
	};

	vk::PipelineLayout CreatePipelineLayout(vk::Device device);

	vk::RenderPass CreateRenderPass(vk::Device device, vk::Format swapchainImageFormat);
	
	GraphicsPipelineOutBundle MakeGraphicsPipeline(GraphicsPipelineInBundle specification);
}
