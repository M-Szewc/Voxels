#pragma once
#include "Core/Base.h"
#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Window.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

namespace VoxelEngine {

	class VO_API App
	{
	public:
		App(int width, int height);
		~App();
		void Run();

		void OnEvent(VOEvent& e);
	private:
		const char* m_ApplicationName = "Voxel application";

		Engine* m_GraphicsEngine;
		std::unique_ptr<Window> m_Window;
		Scene* m_Scene;
		
		double m_LastTime{}, m_CurrentTime{};
		int m_NumFrames{ 0 };
		float m_Framerate{ 0.0f }, m_FrameTime{ 0.0f };

		bool m_Running = true;
	private:
		bool OnWindowCloseEvent(WindowCloseEvent& e);
		void CalculateFrameRate();
	};
}

#define VO_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }