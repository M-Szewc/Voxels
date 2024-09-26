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


		m_Device.destroyPipeline(m_Pipeline);
		m_Device.destroyPipelineLayout(m_PipelineLayout);
		m_Device.destroyRenderPass(m_RenderPass);

		CleanupSwapchain();
		m_Device.destroyCommandPool(m_CommandPool);


		m_Device.destroy();
		//destroy surface
		m_Instance.destroySurfaceKHR(m_Surface);

#ifdef VO_DEBUG
		//destroy debug messanger
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessanger, nullptr, m_DispatchLoaderDY);
#endif

		//destroy Vulkan instance
		m_Instance.destroy();
	}

	void Engine::Render(Scene* scene)
	{
		m_Device.waitForFences(1, &m_SwapchainFrames[m_FrameNumber].InFlightFence, VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vk::ResultValue acquire = m_Device.acquireNextImageKHR(
			m_Swapchain, UINT64_MAX, m_SwapchainFrames[m_FrameNumber].ImageAveilable, nullptr
		);
		imageIndex = acquire.value;


		vk::CommandBuffer commandBuffer = m_SwapchainFrames[m_FrameNumber].CommandBuffer;

		commandBuffer.reset();

		RecordDrawCommands(commandBuffer, imageIndex, scene);

		vk::SubmitInfo submitInfo = {};
		vk::Semaphore waitSemaphores[] = { m_SwapchainFrames[m_FrameNumber].ImageAveilable };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vk::Semaphore signalSemaphores[] = { m_SwapchainFrames[m_FrameNumber].RenderFinished };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		m_Device.resetFences(1, &m_SwapchainFrames[m_FrameNumber].InFlightFence);
		
		try {
			m_GraphicsQueue.submit(submitInfo, m_SwapchainFrames[m_FrameNumber].InFlightFence);
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

		vk::Result present;
		try {
			present = m_PresentQueue.presentKHR(presentInfo);
		}
		catch (vk::OutOfDateKHRError) {
			present = vk::Result::eErrorOutOfDateKHR;
		}

		if (present == vk::Result::eErrorOutOfDateKHR || present == vk::Result::eSuboptimalKHR) {
#ifdef VO_DEBUG
			VO_CORE_INFO("Recreating swapchain");
#endif
			RecreateSwapchain();
			return;
		}

		
		m_FrameNumber = (m_FrameNumber + 1) % m_MaxFramesInFlight;
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
		CreateSwapchain();

		m_FrameNumber = 0;
	}

	void Engine::CreateSwapchain()
	{
		vkInit::SwapChainBundle bundle = vkInit::CreateSwapchain(m_Device, m_PhysicalDevice, m_Surface, m_Width, m_Height);
		m_Swapchain = bundle.Swapchain;
		m_SwapchainFrames = bundle.Frames;
		m_SwapchainFormat = bundle.Format;
		m_SwapchainExtent = bundle.Extent;

		m_MaxFramesInFlight = static_cast<int>(m_SwapchainFrames.size());
	}

	void Engine::RecreateSwapchain()
	{
		m_Width = 0;
		m_Height = 0;
		while (m_Width == 0 || m_Height == 0) {
			glfwGetFramebufferSize(m_Window, &m_Width, &m_Height);
			glfwWaitEvents();
		}

		m_Device.waitIdle();

		CleanupSwapchain();
		CreateSwapchain();
		CreateFramebuffers();
		CreateFrameSyncObjects();
		vkInit::CommandBufferInputChunk commandBufferInput = { m_Device, m_CommandPool, m_SwapchainFrames };
		vkInit::CreateFrameCommandBuffers(commandBufferInput);
	
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
		m_PipelineLayout = output.m_PipelineLayout;
		m_RenderPass = output.m_RenderPass;
		m_Pipeline = output.m_Pipeline;
	}

	void Engine::FinalizeSetup()
	{
		CreateFramebuffers();

		m_CommandPool = vkInit::CreateCommandPool(m_Device, m_PhysicalDevice, m_Surface);

		vkInit::CommandBufferInputChunk commandBufferInput = { m_Device, m_CommandPool, m_SwapchainFrames };
		m_MainCommandBuffer = vkInit::CreateCommandBuffer(commandBufferInput);
	
		vkInit::CreateFrameCommandBuffers(commandBufferInput);

		CreateFrameSyncObjects();
	}

	void Engine::CreateFramebuffers()
	{
		vkInit::FramebufferInput framebufferInput;
		framebufferInput.device = m_Device;
		framebufferInput.renderPass = m_RenderPass;
		framebufferInput.swapchainExtent = m_SwapchainExtent;

		vkInit::CreateFramebuffers(framebufferInput, m_SwapchainFrames);

	}

	void Engine::CreateFrameSyncObjects()
	{
		for (vkUtil::SwapChainFrame& frame : m_SwapchainFrames) {
			frame.InFlightFence = vkInit::CreateFence(m_Device);
			frame.ImageAveilable = vkInit::CreateSemaphore(m_Device);
			frame.RenderFinished = vkInit::CreateSemaphore(m_Device);
		}
	}

	void Engine::RecordDrawCommands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene)
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
		
		for (glm::vec3 position : scene->m_TrianglePositions) {
			glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
			vkUtil::ObjectData objectData;
			objectData.model = model;
			commandBuffer.pushConstants(m_PipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(vkUtil::ObjectData), & objectData);
			commandBuffer.draw(3, 1, 0, 0);
		}

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
	void Engine::CleanupSwapchain()
	{
		//destroy objects connected with swapchain
		for (vkUtil::SwapChainFrame& frame : m_SwapchainFrames) {
			m_Device.freeCommandBuffers(m_CommandPool, frame.CommandBuffer);
			
			m_Device.destroyImageView(frame.ImageView);
			m_Device.destroyFramebuffer(frame.FrameBuffer);

			m_Device.destroyFence(frame.InFlightFence);
			m_Device.destroySemaphore(frame.ImageAveilable);
			m_Device.destroySemaphore(frame.RenderFinished);
		}

		//destroy device
		m_Device.destroySwapchainKHR(m_Swapchain);
	}
}