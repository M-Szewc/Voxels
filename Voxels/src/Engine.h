#pragma once

#include "vopch.h"

namespace Game {

	class Engine
	{
	public:
		Engine();
		~Engine();
	private:
		int m_width{ 640 };
		int m_height{ 480 };

		const char* m_applicationName = "Voxel application";

		GLFWwindow* window{ nullptr };

		vk::Instance m_instance{ nullptr };

	private:
		void InitializeWindow();
		void CreateVulkanInstance();
	};

}
