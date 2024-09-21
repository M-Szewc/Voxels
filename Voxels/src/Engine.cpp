#include "vopch.h"

#include "Engine.h"

#include "VulkanInstance.h"
#include "VulkanLogger.h"
#include "VulkanDevice.h"
#include "Swapchain.h"
#include "Pipeline.h"
#include "FrameBuffer.h"
#include "Commands.h"
#include "Sync.h"

namespace Game {
	Engine::Engine(int width, int height, GLFWwindow* window):
	m_Width(width),m_Height(height),m_Window(window)
	{
		VO_CORE_FATAL("hello");

#ifdef VO_DEBUG
		VO_CORE_TRACE("Initialized engine logger");
		VO_CORE_TRACE("Creating a graphics engine");
#endif

		m_Width = 640;
		m_Height = 480;

		CreateVulkanInstance();
		SetupDevice();
		SetupPipeline();
		FinalizeSetup();
	}


	Engine::~Engine()
	{
		m_Device.waitIdle();

#ifdef VO_DEBUG
		VO_CORE_TRACE("Destroying graphics engine");
#endif

		m_Device.destroyFence(m_InFlightFence);
		m_Device.destroySemaphore(m_ImageAveilable);
		m_Device.destroySemaphore(m_RenderFinished);

		m_Device.destroyCommandPool(m_CommandPool);

		m_Device.destroyPipeline(m_Pipeline);
		m_Device.destroyPipelineLayout(m_Layout);
		m_Device.destroyRenderPass(m_RenderPass);

		//destroy image views
		for (vkUtil::SwapChainFrame frame : m_SwapchainFrames) {
			m_Device.destroyImageView(frame.ImageView);
			m_Device.destroyFramebuffer(frame.FrameBuffer);
		}

		//destroy device
		m_Device.destroySwapchainKHR(m_Swapchain);
		m_Device.destroy();
		//destroy surface
		m_Instance.destroySurfaceKHR(m_Surface);

#ifdef VO_DEBUG
		//destroy debug messanger
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessanger, nullptr, m_DispatchLoaderDY);
#endif

		//destroy Vulkan instance
		m_Instance.destroy();
		//stop glfw
		glfwTerminate();
	}

	void Engine::Render()
	{
		m_Device.waitForFences(1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
		m_Device.resetFences(1, &m_InFlightFence);

		uint32_t imageIndex{ m_Device.acquireNextImageKHR(m_Swapchain, UINT64_MAX, m_ImageAveilable, nullptr).value };

		vk::CommandBuffer commandBuffer = m_SwapchainFrames[imageIndex].CommandBuffer;

		commandBuffer.reset();

		RecordDrawCommands(commandBuffer, imageIndex);

		vk::SubmitInfo submitInfo = {};
		vk::Semaphore waitSemaphores[] = { m_ImageAveilable };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vk::Semaphore signalSemaphores[] = { m_RenderFinished };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		try {
			m_GraphicsQueue.submit(submitInfo, m_InFlightFence);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to submit draw command buffer");
#endif
		}

		vk::PresentInfoKHR presentInfo = {};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		vk::SwapchainKHR swapchains[] = { m_Swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;

		m_PresentQueue.presentKHR(presentInfo);
		
	}

	// create and setup Vulkan instance
	void Engine::CreateVulkanInstance()
	{
		m_Instance = vkInit::MakeInstance(m_ApplicationName);
		m_DispatchLoaderDY = vk::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);

		//create debug messanger
#ifdef VO_DEBUG
		m_DebugMessanger = vkInit::CreateDebugMessanger(m_Instance, m_DispatchLoaderDY);
#endif

		VkSurfaceKHR c_style_surface;
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &c_style_surface) != VK_SUCCESS) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to abstract the glfw surface for Vulkan");
#endif

		}
		else {
#ifdef VO_DEBUG
			VO_CORE_TRACE("Successfully abstracted the glfw surface for Vulkan");
#endif

		}
		m_Surface = c_style_surface;
	}

	// setup device - get physical device and abstract it
	void Engine::SetupDevice()
	{
		m_PhysicalDevice = vkInit::GetPhysicalDevice(m_Instance);
		m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface);
		std::array<vk::Queue, 2> queues = vkInit::GetQueue(m_PhysicalDevice, m_Device, m_Surface);
		m_GraphicsQueue = queues[0];
		m_PresentQueue = queues[1];

		// create swapchain
		vkInit::SwapChainBundle bundle = vkInit::CreateSwapchain(m_Device, m_PhysicalDevice, m_Surface, m_Width, m_Height);
		m_Swapchain = bundle.Swapchain;
		m_SwapchainFrames = bundle.Frames;
		m_SwapchainFormat = bundle.Format;
		m_SwapchainExtent = bundle.Extent;
	}
	void Engine::SetupPipeline()
	{
		vkInit::GraphicsPipelineInBundle specification = {};
		specification.m_Device = m_Device;
		specification.m_VertexFilepath = "shaders/vertex.spv";
		specification.m_FragmentFilepath = "shaders/fragment.spv";
		specification.m_SwapchainExtent = m_SwapchainExtent;
		specification.m_SwapchainImageFormat = m_SwapchainFormat;

		vkInit::GraphicsPipelineOutBundle output = vkInit::MakeGraphicsPipeline(specification);
		m_Layout = output.m_Layout;
		m_RenderPass = output.m_RenderPass;
		m_Pipeline = output.m_Pipeline;
	}
	void Engine::FinalizeSetup()
	{
		vkInit::FramebufferInput framebufferInput;
		framebufferInput.device = m_Device;
		framebufferInput.renderPass = m_RenderPass;
		framebufferInput.swapchainExtent = m_SwapchainExtent;

		vkInit::CreateFramebuffers(framebufferInput, m_SwapchainFrames);
	
		m_CommandPool = vkInit::CreateCommandPool(m_Device, m_PhysicalDevice, m_Surface);
	
		VO_CORE_INFO("size: {0}", m_SwapchainFrames.size());

		vkInit::CommandBufferInputChunk commandBufferInput = { m_Device, m_CommandPool, m_SwapchainFrames };
		m_MainCommandBuffer = vkInit::CreateCommandBuffers(commandBufferInput);
	
		m_InFlightFence = vkInit::CreateFence(m_Device);
		m_ImageAveilable = vkInit::CreateSemaphore(m_Device);
		m_RenderFinished = vkInit::CreateSemaphore(m_Device);
	}

	void Engine::RecordDrawCommands(vk::CommandBuffer commandBuffer, uint32_t imageIndex)
	{
		vk::CommandBufferBeginInfo beginInfo = {};
		try {
			commandBuffer.begin(beginInfo);
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to begin recording command buffer");
#endif
		}

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapchainFrames[imageIndex].FrameBuffer;
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = m_SwapchainExtent;
		vk::ClearValue clearColor = { std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

		commandBuffer.draw(3, 1, 0, 0);

		commandBuffer.endRenderPass();

		try {
			commandBuffer.end();
		}
		catch (vk::SystemError err) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to finish recording command buffer");
#endif
		}

	}
}