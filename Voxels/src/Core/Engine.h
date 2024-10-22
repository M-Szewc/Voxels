#pragma once

#include "Core/Base.h"
#include "Core/Scene.h"
#include "vkUtil/VertexManager.h"

#include "vkUtil/VulkanUtil.h"

namespace VoxelEngine {

	class Engine
	{
	public:
		Engine(int width, int height, GLFWwindow* window);
		~Engine();

		void Render(Scene* scene);

	private:
		int m_Width;
		int m_Height;

		const char* m_ApplicationName = "Voxel application";

		GLFWwindow* m_Window{ nullptr };

		//vulkan instance
		vk::Instance m_Instance{ nullptr };
		vk::DebugUtilsMessengerEXT m_DebugMessanger{ nullptr };
		vk::DispatchLoaderDynamic m_DispatchLoaderDY;
		vk::SurfaceKHR m_Surface;

		//device-related
		vk::PhysicalDevice m_PhysicalDevice{ nullptr };
		vk::Device m_Device{ nullptr };
		vk::Queue m_GraphicsQueue{ nullptr };
		vk::Queue m_PresentQueue{ nullptr };
		vk::SwapchainKHR m_Swapchain;
		std::vector<vkUtil::SwapChainFrame> m_SwapchainFrames;
		vk::Format m_SwapchainFormat;
		vk::Extent2D m_SwapchainExtent;

		//pipeline-related
		vk::PipelineLayout m_PipelineLayout;
		vk::RenderPass m_RenderPass;
		vk::Pipeline m_Pipeline;

		//command-related
		vk::CommandPool m_CommandPool;
		vk::CommandBuffer m_MainCommandBuffer;

		//synchronization-related
		int m_MaxFramesInFlight;
		int m_FrameNumber;

		//descriptor objects
		vk::DescriptorSetLayout m_DescriptorSetLayout;
		vk::DescriptorPool m_DescriptorPool;

		//asset pointers
		vkUtil::VertexManager* m_Meshes;

	private:
		void CreateVulkanInstance();
		void SetupDevice();
		void CreateSwapchain();
		void RecreateSwapchain();

		void CreateDescriptorSetLayout();
		void SetupPipeline();

		void FinalizeSetup();
		void CreateFramebuffers();
		void CreateFrameResources();

		void CreateAssets();
		
		void PrepareScene(vk::CommandBuffer commandbuffer);
		void PrepareFrame(uint32_t imageIndex, Scene* scene);
		void RecordDrawCommands(vk::CommandBuffer commandBuffer, uint32_t imageIndex, Scene* scene);
	
		void CleanupSwapchain();
	};

}
