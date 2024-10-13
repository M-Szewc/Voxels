#include "vopch.h"

#include "Core/Engine.h"

#include "vkInit/VulkanInstance.h"
#include "vkInit/VulkanLogger.h"
#include "vkInit/VulkanDevice.h"
#include "vkInit/Swapchain.h"
#include "vkInit/Pipeline.h"
#include "vkInit/FrameBuffer.h"
#include "vkInit/Commands.h"
#include "vkInit/Sync.h"
#include "vkUtil/RenderUtil.h"
#include "vkInit/Descriptors.h"

namespace VoxelEngine {
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

		CreateDescriptorSetLayout();
		SetupPipeline();
		FinalizeSetup();

		CreateAssets();
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

		m_Device.destroyDescriptorSetLayout(m_DescriptorSetLayout);

		delete m_Meshes;

		m_Device.destroy();
		//destroy surface
		m_Instance.destroySurfaceKHR(m_Surface);

		//destroy debug messanger
#ifdef USE_VALIDATION_LAYER
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessanger, nullptr, m_DispatchLoaderDY);
#endif

		//destroy Vulkan instance
		m_Instance.destroy();
	}

	void Engine::Render(Scene* scene)
	{
		m_Device.waitForFences(1, &m_SwapchainFrames[m_FrameNumber].m_InFlightFence, VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		vk::ResultValue acquire = m_Device.acquireNextImageKHR(
			m_Swapchain, UINT64_MAX, m_SwapchainFrames[m_FrameNumber].m_ImageAveilable, nullptr
		);
		imageIndex = acquire.value;


		vk::CommandBuffer commandBuffer = m_SwapchainFrames[m_FrameNumber].m_CommandBuffer;

		commandBuffer.reset();

		PrepareFrame(imageIndex, scene);

		RecordDrawCommands(commandBuffer, imageIndex, scene);

		vk::SubmitInfo submitInfo = {};
		vk::Semaphore waitSemaphores[] = { m_SwapchainFrames[m_FrameNumber].m_ImageAveilable };
		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		vk::Semaphore signalSemaphores[] = { m_SwapchainFrames[m_FrameNumber].m_RenderFinished };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		m_Device.resetFences(1, &m_SwapchainFrames[m_FrameNumber].m_InFlightFence);
		
		try {
			m_GraphicsQueue.submit(submitInfo, m_SwapchainFrames[m_FrameNumber].m_InFlightFence);
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

		//create debug messanger
#ifdef USE_VALIDATION_LAYER
		m_DispatchLoaderDY = vk::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);
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
		CreateFrameResources();
		vkInit::CommandBufferInputChunk commandBufferInput = { m_Device, m_CommandPool, m_SwapchainFrames };
		vkInit::CreateFrameCommandBuffers(commandBufferInput);
	
	}

	void Engine::CreateDescriptorSetLayout()
	{
		vkInit::DescriptorSetLayoutData bindings;
		bindings.Count = 2;

		bindings.Indices.push_back(0);
		bindings.Types.push_back(vk::DescriptorType::eUniformBuffer);
		bindings.Counts.push_back(1);
		bindings.Stages.push_back(vk::ShaderStageFlagBits::eVertex);

		bindings.Indices.push_back(1);
		bindings.Types.push_back(vk::DescriptorType::eStorageBuffer);
		bindings.Counts.push_back(1);
		bindings.Stages.push_back(vk::ShaderStageFlagBits::eVertex);

		m_DescriptorSetLayout = vkInit::CreateDescriptorSetLayout(m_Device, bindings);
	}

	void Engine::SetupPipeline()
	{
		vkInit::GraphicsPipelineInBundle specification = {};
		specification.Device = m_Device;
		specification.VertexFilepath = "shaders/compiled/vertex.spv";
		specification.FragmentFilepath = "shaders/compiled/fragment.spv";
		specification.SwapchainExtent = m_SwapchainExtent;
		specification.SwapchainImageFormat = m_SwapchainFormat;
		specification.DescriptorSetLayout = m_DescriptorSetLayout;

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

		CreateFrameResources();
	}

	void Engine::CreateFramebuffers()
	{
		vkInit::FramebufferInput framebufferInput;
		framebufferInput.Device = m_Device;
		framebufferInput.RenderPass = m_RenderPass;
		framebufferInput.SwapchainExtent = m_SwapchainExtent;

		vkInit::CreateFramebuffers(framebufferInput, m_SwapchainFrames);

	}

	void Engine::CreateFrameResources()
	{
		vkInit::DescriptorSetLayoutData bindings;
		bindings.Count = 2;
		bindings.Types.push_back(vk::DescriptorType::eUniformBuffer);
		bindings.Types.push_back(vk::DescriptorType::eStorageBuffer);
		m_DescriptorPool = vkInit::CreateDescriptorPool(m_Device, static_cast<uint32_t>(m_SwapchainFrames.size()), bindings);

		for (vkUtil::SwapChainFrame& frame : m_SwapchainFrames) {
			frame.m_InFlightFence = vkInit::CreateFence(m_Device);
			frame.m_ImageAveilable = vkInit::CreateSemaphore(m_Device);
			frame.m_RenderFinished = vkInit::CreateSemaphore(m_Device);
			
			frame.CreateDescriptorResources(m_Device, m_PhysicalDevice);
		
			frame.m_DescriptorSet = vkInit::AllocateDescriptorSet(m_Device, m_DescriptorPool, m_DescriptorSetLayout);
		}
	}

	void Engine::CreateAssets()
	{
		m_Meshes = new vkUtil::VertexManager();

		std::vector<float> vertices = { {
			 0.0f, -0.05f, 0.0f, 1.0f, 0.0f,
			 0.05f, 0.05f, 0.0f, 1.0f, 0.0f,
			-0.05f, 0.05f, 0.0f, 1.0f, 0.0f
		} };
		vkUtil::MeshType type = vkUtil::MeshType::TRIANGLE;

		m_Meshes->Consume(type, vertices);

		vertices = { {
			 -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
			 -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
			  0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
			  0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
			  0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
			 -0.05f,  0.05f, 1.0f, 0.0f, 0.0f,
		} };
		type = vkUtil::MeshType::SQUARE;

		m_Meshes->Consume(type, vertices);

		vertices = { {
			  0.0f,  -0.05f, 0.0f, 0.0f, 1.0f,
			  0.05f, -0.01f, 0.0f, 0.0f, 1.0f,
			 -0.05f, -0.01f, 0.0f, 0.0f, 1.0f,
			 -0.05f, -0.01f, 0.0f, 0.0f, 1.0f,
			  0.05f, -0.01f, 0.0f, 0.0f, 1.0f,
			  0.03f,  0.05f, 0.0f, 0.0f, 1.0f,
			 -0.05f, -0.01f, 0.0f, 0.0f, 1.0f,
			  0.03f,  0.05f, 0.0f, 0.0f, 1.0f,
			 -0.03f,  0.05f, 0.0f, 0.0f, 1.0f,
		} };
		type = vkUtil::MeshType::PENTAGON;

		m_Meshes->Consume(type, vertices);

		vkUtil::FinalizationChunk finalizationChunk;
		finalizationChunk.LogicalDevice = m_Device;
		finalizationChunk.PhysicalDevice = m_PhysicalDevice;
		finalizationChunk.Queue = m_GraphicsQueue;
		finalizationChunk.CommandBuffer = m_MainCommandBuffer;

		m_Meshes->Finalize(finalizationChunk);
	}

	void Engine::PrepareScene(vk::CommandBuffer commandbuffer)
	{
		vk::Buffer vertexBuffers[] = { m_Meshes->m_VertexBuffer.m_Buffer };
		vk::DeviceSize offsets[] = { 0 };
		commandbuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	}

	void Engine::PrepareFrame(uint32_t imageIndex, Scene* scene)
	{

		vkUtil::SwapChainFrame& _frame = m_SwapchainFrames[imageIndex];

		glm::vec3 eye = { 1.0f, 0.0f, -1.0f };
		glm::vec3 center = { 0.0f, 0.0f, 0.0f };
		glm::vec3 up = { 0.0f, 0.0f, -1.0f };
		glm::mat4 view = glm::lookAt(eye, center, up);

		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(m_SwapchainExtent.width) / static_cast<float>(m_SwapchainExtent.height),
			0.1f, 10.0f
		);

		projection[1][1] *= -1;

		_frame.m_CameraData.view = view;
		_frame.m_CameraData.projection = projection;
		_frame.m_CameraData.viewProjection = projection * view;
		memcpy(
			_frame.m_CameraDataWriteLocation,
			&(_frame.m_CameraData),
			sizeof(vkUtil::UniformBufferObject)
		);

		size_t i = 0;
		for (glm::vec3& position : scene->m_TrianglePositions) {
			_frame.m_ModelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
		}
		for (glm::vec3& position : scene->m_SquarePositions) {
			_frame.m_ModelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
		}
		for (glm::vec3& position : scene->m_PentagonPositions) {
			_frame.m_ModelTransforms[i++] = glm::translate(glm::mat4(1.0f), position);
		}

		memcpy(
			_frame.m_ModelBufferWriteLocation,
			_frame.m_ModelTransforms.data(),
			i * sizeof(glm::mat4)
		);

		_frame.WriteDescriptorSet(m_Device);
		
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
		renderPassInfo.framebuffer = m_SwapchainFrames[imageIndex].m_FrameBuffer;
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = m_SwapchainExtent;
		vk::ClearValue clearColor = { std::array<float, 4>{1.0f, 0.5f, 0.25f, 1.0f} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

		commandBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics, 
			m_PipelineLayout, 0, m_SwapchainFrames[imageIndex].m_DescriptorSet,
			nullptr
		);

		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
		
		PrepareScene(commandBuffer);
		int firstVertex = m_Meshes->m_Offsets.find(vkUtil::MeshType::TRIANGLE)->second;
		int vertexCount = m_Meshes->m_Sizes.find(vkUtil::MeshType::TRIANGLE)->second;
		uint32_t startInstance = 0;
		uint32_t instanceCount = static_cast<uint32_t>(scene->m_TrianglePositions.size());
		commandBuffer.draw(vertexCount, instanceCount, firstVertex, startInstance);

		startInstance += instanceCount;
		instanceCount = static_cast<uint32_t>(scene->m_SquarePositions.size());
		firstVertex = m_Meshes->m_Offsets.find(vkUtil::MeshType::SQUARE)->second;
		vertexCount = m_Meshes->m_Sizes.find(vkUtil::MeshType::SQUARE)->second;
		commandBuffer.draw(vertexCount, instanceCount, firstVertex, startInstance);

		startInstance += instanceCount;
		instanceCount = static_cast<uint32_t>(scene->m_PentagonPositions.size());
		firstVertex = m_Meshes->m_Offsets.find(vkUtil::MeshType::PENTAGON)->second;
		vertexCount = m_Meshes->m_Sizes.find(vkUtil::MeshType::PENTAGON)->second;
		commandBuffer.draw(vertexCount, instanceCount, firstVertex, startInstance);

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
			m_Device.freeCommandBuffers(m_CommandPool, frame.m_CommandBuffer);
			
			m_Device.destroyImageView(frame.m_ImageView);
			m_Device.destroyFramebuffer(frame.m_FrameBuffer);

			m_Device.destroyFence(frame.m_InFlightFence);
			m_Device.destroySemaphore(frame.m_ImageAveilable);
			m_Device.destroySemaphore(frame.m_RenderFinished);
		
			m_Device.unmapMemory(frame.m_CameraDataBuffer.m_BufferMemory);
			m_Device.freeMemory(frame.m_CameraDataBuffer.m_BufferMemory);
			m_Device.destroyBuffer(frame.m_CameraDataBuffer.m_Buffer);
		
			m_Device.unmapMemory(frame.m_ModelBuffer.m_BufferMemory);
			m_Device.freeMemory(frame.m_ModelBuffer.m_BufferMemory);
			m_Device.destroyBuffer(frame.m_ModelBuffer.m_Buffer);

		}

		//destroy device
		m_Device.destroySwapchainKHR(m_Swapchain);
	
		m_Device.destroyDescriptorPool(m_DescriptorPool);
	}
}