#include "vopch.h"
#include "Engine.h"

namespace Game {
	Engine::Engine() {
#ifdef VO_DEBUG
		std::cout << "Creating a graphics engine" << std::endl;
#endif
		m_width = 640;
		m_height = 420;

		InitializeWindow();
	}
	Engine::~Engine()
	{
#ifdef VO_DEBUG
		std::cout << "Destroying graphics engine" << std::endl;
#endif
		//stop glfw
		glfwTerminate();
	}
	void Engine::InitializeWindow()
	{
		//initialize glfw
		glfwInit();

		//no default rendering clinent - we will hook up vulkan later
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//resizing doesn't work now, so disable it
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		if (window = glfwCreateWindow(m_width, m_height, "Voxels", nullptr, nullptr)) {
#ifdef VO_DEBUG
			std::cout << "Created window successfully" << std::endl;
#endif
		}
		else {
#ifdef VO_DEBUG
			std::cout << "Failed to create window" << std::endl;
#endif
		}
	}
}