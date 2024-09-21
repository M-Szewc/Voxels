#include "vopch.h"
#include "App.h"

namespace Game {

	App::App(int width, int height)
	{
		// initialize loggers
		Log::Init();
		BuildGLFWWindow(width, height);

		m_LastTime = glfwGetTime();

		m_GraphicsEngine = new Engine(width, height, m_Window);
	}

	App::~App()
	{
		delete m_GraphicsEngine;

		//stop glfw
		glfwTerminate();
	}

	void App::Run()
	{
		while (!glfwWindowShouldClose(m_Window)) {
			glfwPollEvents();
			m_GraphicsEngine->Render();
			CalculateFrameRate();
		}
	}

	void App::BuildGLFWWindow(int width, int height)
	{
		//initialize glfw
		glfwInit();
		//no default rendering clinent - we will hook up vulkan later
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//resizing doesn't work now, so disable it
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// try to create a window
		if (m_Window = glfwCreateWindow(width, height, m_ApplicationName, nullptr, nullptr)) {
#ifdef VO_DEBUG
			VO_CORE_TRACE("Created window successfully");
#endif

		}
		else {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create window");
#endif

		}
	}

	void App::CalculateFrameRate()
	{
		m_CurrentTime = glfwGetTime();
		double delta = m_CurrentTime - m_LastTime;
		
		if (delta >= 1) {
			int framerate{ std::max(1,int(m_NumFrames / delta)) };
			std::stringstream title;
			title << m_ApplicationName << " Running at " << framerate << " fps.";
			glfwSetWindowTitle(m_Window, title.str().c_str());
			m_LastTime = m_CurrentTime;
			m_NumFrames = -1;
			m_FrameTime = float(1000.0 / framerate);
		}

		++m_NumFrames;
	}
}
