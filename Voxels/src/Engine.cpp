#include "vopch.h"

#include "Engine.h"

#include "VulkanInstance.h"
#include "VulkanLogger.h"

namespace Game {
	Engine::Engine() {
		Log::Init();
#ifdef VO_DEBUG
		VO_CORE_INFO("Initialized engine logger");
		VO_CORE_INFO("Creating a graphics engine");
#endif
		VO_CORE_TRACE("normal");
		VO_CORE_INFO("info");
		VO_CORE_WARN("warning");
		VO_CORE_ERROR("error");
		VO_CORE_FATAL("fatal");
		m_Width = 640;
		m_Height = 480;

		InitializeWindow();
		CreateVulkanInstance();
		CreateDebugMessanger();
	}


	Engine::~Engine()
	{
#ifdef VO_DEBUG
		VO_CORE_INFO("Destroying graphics engine");
#endif

		//destroy debug messanger
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessanger, nullptr, m_DispatchLoaderDY);

		//destroy Vulkan instance
		m_Instance.destroy();

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

		if (m_Window = glfwCreateWindow(m_Width, m_Height, m_ApplicationName, nullptr, nullptr)) {
#ifdef VO_DEBUG
			VO_CORE_INFO("Created window successfully");
#endif
		}
		else {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create window");
#endif
		}
	}


	void Engine::CreateVulkanInstance()
	{
		m_Instance = vkInit::MakeInstance(m_ApplicationName);
		m_DispatchLoaderDY = vk::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);
	}


	void Engine::CreateDebugMessanger()
	{
		m_DebugMessanger = vkInit::CreateDebugMessanger(m_Instance, m_DispatchLoaderDY);
	}
}