#pragma once
#include "Core/Base.h"

namespace vkInit {

	struct GraphicsPipelineInBundle {
		vk::Device Device;
		std::string VertexFilepath;
		std::string FragmentFilepath;
		vk::Extent2D SwapchainExtent;
		vk::Format SwapchainImageFormat;
		vk::DescriptorSetLayout DescriptorSetLayout;
	};

	struct GraphicsPipelineOutBundle {
		vk::PipelineLayout m_PipelineLayout;
		vk::RenderPass m_RenderPass;
		vk::Pipeline m_Pipeline;
	};

	vk::PipelineLayout CreatePipelineLayout(vk::Device device, vk::DescriptorSetLayout layout);

	vk::RenderPass CreateRenderPass(vk::Device device, vk::Format swapchainImageFormat);
	
	GraphicsPipelineOutBundle MakeGraphicsPipeline(GraphicsPipelineInBundle specification);
}
