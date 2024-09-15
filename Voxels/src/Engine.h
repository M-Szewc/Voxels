#pragma once

#include "vopch.h"

namespace Game {

	class Engine
	{
	public:
		Engine();
		~Engine();
	private:
		int m_Width{ 640 };
		int m_Height{ 480 };

		const char* m_ApplicationName = "Voxel application";

		GLFWwindow* m_Window{ nullptr };

		//vulkan instance
		vk::Instance m_Instance{ nullptr };
		vk::DebugUtilsMessengerEXT m_DebugMessanger{ nullptr };
		vk::DispatchLoaderDynamic m_DispatchLoaderDY;

		//devices
		vk::PhysicalDevice m_PhysicalDevice{ nullptr };
		vk::Device m_Device{ nullptr };
		vk::Queue m_GraphicsQueue{ nullptr };

	private:
		void InitializeWindow();
		void CreateVulkanInstance();
		void SetupDevice();
	};

}
