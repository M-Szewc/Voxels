#pragma once
#include "Base.h"
#include "Engine.h"
#include "Scene.h"

namespace Game {

	class App
	{
	public:
		App(int width, int height);
		~App();
		void Run();
	private:
		const char* m_ApplicationName = "Voxel application";

		Engine* m_GraphicsEngine;
		GLFWwindow* m_Window;
		Scene* m_Scene;
		
		double m_LastTime, m_CurrentTime;
		int m_NumFrames{ 0 };
		float m_Framerate{ 0.0f }, m_FrameTime{ 0.0f };
	private:
		void BuildGLFWWindow(int width, int height);
		void CalculateFrameRate();
	};
}
