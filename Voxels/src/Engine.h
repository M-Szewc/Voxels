#pragma once

#include "vopch.h"
#include <GLFW/glfw3.h>

namespace Game {

	class Engine
	{
	public:
		Engine();
		~Engine();
	private:
		int m_width, m_height;

		GLFWwindow* window;
	private:
		void InitializeWindow();
	};

}
