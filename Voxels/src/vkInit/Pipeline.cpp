#include "vopch.h"

#include "Pipeline.h"

#include "vkUtil/RenderUtil.h"
#include "shaders/Shaders.h"
#include "vkMesh/Mesh.h"

namespace vkInit {

	vk::PipelineLayout CreatePipelineLayout(vk::Device device, vk::DescriptorSetLayout layout) {

		vk::PipelineLayoutCreateInfo layoutInfo;
		layoutInfo.flags = vk::PipelineLayoutCreateFlags();
		
		layoutInfo.setLayoutCount = 1;
		layoutInfo.pSetLayouts = &layout;

		layoutInfo.pushConstantRangeCount = 0;

		try {
			return device.createPipelineLayout(layoutInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create pipeline layout!");
#endif
		}
	}

	vk::RenderPass CreateRenderPass(vk::Device device, vk::Format swapchainImageFormat) {

		vk::AttachmentDescription colorAttachment = {};
		colorAttachment.flags = vk::AttachmentDescriptionFlags();
		colorAttachment.format = swapchainImageFormat;
		colorAttachment.samples = vk::SampleCountFlagBits::e1;
		colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
		colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
		colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
		colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass = {};
		subpass.flags = vk::SubpassDescriptionFlags();
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		vk::RenderPassCreateInfo renderpassInfo = {};
		renderpassInfo.flags = vk::RenderPassCreateFlags();
		renderpassInfo.attachmentCount = 1;
		renderpassInfo.pAttachments = &colorAttachment;
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpass;

		try {
			return device.createRenderPass(renderpassInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create renderpass!");
#endif
		}
	}

	GraphicsPipelineOutBundle MakeGraphicsPipeline(GraphicsPipelineInBundle specification) {

		vk::GraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.flags = vk::PipelineCreateFlags();

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

		//Vertex Input
		vk::VertexInputBindingDescription bindingDescription = vkMesh::GetPosColorBindingDescription();
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = vkMesh::GetPosColorAttributeDescriptions();
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = 2;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;

		//Input Assembly
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
		inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
		inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

		//Vertex Shader
#ifdef VO_DEBUG
		VO_CORE_TRACE("Create vertex shader module");
#endif
		vk::ShaderModule vertexShader = vkUtil::CreateModule(specification.VertexFilepath, specification.Device);
		vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
		vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
		vertexShaderInfo.module = vertexShader;
		vertexShaderInfo.pName = "main";
		shaderStages.push_back(vertexShaderInfo);

		//Viewport and Scissor
		vk::Viewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = specification.SwapchainExtent.width;
		viewport.height = specification.SwapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vk::Rect2D scissor = {};
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		scissor.extent = specification.SwapchainExtent;
		vk::PipelineViewportStateCreateInfo viewportState = {};
		viewportState.flags = vk::PipelineViewportStateCreateFlags();
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
		pipelineInfo.pViewportState = &viewportState;

		//Rasterizer
		vk::PipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;
		rasterizer.depthBiasEnable = VK_FALSE;
		pipelineInfo.pRasterizationState = &rasterizer;

		//Fragment Shader
#ifdef VO_DEBUG
		VO_CORE_TRACE("Create fragment shader module");
#endif
		vk::ShaderModule fragmentShader = vkUtil::CreateModule(specification.FragmentFilepath, specification.Device);
		vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
		fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
		fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
		fragmentShaderInfo.module = fragmentShader;
		fragmentShaderInfo.pName = "main";
		shaderStages.push_back(fragmentShaderInfo);
		pipelineInfo.stageCount = shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();

		//Multisampling
		vk::PipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		pipelineInfo.pMultisampleState = &multisampling;

		//Color Blend
		vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;
		vk::PipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		pipelineInfo.pColorBlendState = &colorBlending;

		//Pipeline Layout
#ifdef VO_DEBUG
		VO_CORE_TRACE("Create Pipeline Layout");
#endif
		vk::PipelineLayout layout = CreatePipelineLayout(specification.Device, specification.DescriptorSetLayout);
		pipelineInfo.layout = layout;

		//Renderpass
#ifdef VO_DEBUG
		VO_CORE_TRACE("Create RenderPass");
#endif
		vk::RenderPass renderPass = CreateRenderPass(specification.Device, specification.SwapchainImageFormat);
		pipelineInfo.renderPass = renderPass;

		//Flags
		pipelineInfo.basePipelineHandle = nullptr;

		//Make the pipeline
#ifdef VO_DEBUG
		VO_CORE_TRACE("Create Graphics Pipeline");
#endif
		vk::Pipeline graphicsPipeline;
		try {
			graphicsPipeline = (specification.Device.createGraphicsPipeline(nullptr, pipelineInfo)).value;
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create Graphics Pipeline!");
#endif
		}

		GraphicsPipelineOutBundle output = {};
		output.m_PipelineLayout = layout;
		output.m_RenderPass = renderPass;
		output.m_Pipeline = graphicsPipeline;


		specification.Device.destroyShaderModule(vertexShader);
		specification.Device.destroyShaderModule(fragmentShader);
		return output;
	}
}