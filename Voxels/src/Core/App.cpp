#include "vopch.h"
#include "Core/App.h"

namespace VoxelEngine {

	App::App(int width, int height)
		:m_Running{true}
	{
		// initialize loggers
		Log::Init();

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(VO_BIND_EVENT_FN(App::OnEvent));

		m_LastTime = glfwGetTime();

		m_GraphicsEngine = new Engine(width, height, (GLFWwindow*)m_Window->GetWindow());
	
		m_Scene = new Scene();
	}

	App::~App()
	{
		delete m_GraphicsEngine;
		delete m_Scene;

		//stop glfw
		glfwTerminate();
	}

	void App::Run()
	{
		while (m_Running) {
			glfwPollEvents();
			m_GraphicsEngine->Render(m_Scene);
			m_Window->OnUpdate();
			CalculateFrameRate();
		}
	}

	void App::OnEvent(VOEvent& e)
	{
#ifdef VO_TRACE_EVENTS
		VO_CORE_TRACE("{0}",e);
#endif
		VOEventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(VO_BIND_EVENT_FN(App::OnWindowCloseEvent));
	}

	bool App::OnWindowCloseEvent(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void App::CalculateFrameRate()
	{
		m_CurrentTime = glfwGetTime();
		double delta = m_CurrentTime - m_LastTime;
		
		if (delta >= 1) {
			int framerate{ std::max(1,int(m_NumFrames / delta)) };
			std::stringstream title;
			title << m_ApplicationName << " Running at " << framerate << " fps.";
			m_Window->SetTitle(title.str());
			m_LastTime = m_CurrentTime;
			m_NumFrames = -1;
			m_FrameTime = float(1000.0 / framerate);
		}

		++m_NumFrames;
	}
}
