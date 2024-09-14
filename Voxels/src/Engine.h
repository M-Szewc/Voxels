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

		//debug vulkan callback
		vk::DebugUtilsMessengerEXT m_DebugMessanger{ nullptr };
		//dynamic instance dispatcher
		vk::DispatchLoaderDynamic m_DispatchLoaderDY;

	private:
		void InitializeWindow();
		void CreateVulkanInstance();
		void CreateDebugMessanger();
	};

}
