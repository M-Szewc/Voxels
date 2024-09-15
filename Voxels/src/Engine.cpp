#include "vopch.h"

#include "Engine.h"

#include "VulkanInstance.h"
#include "VulkanLogger.h"
#include "VulkanDevice.h"

namespace Game {
	Engine::Engine() {
		Log::Init();
#ifdef VO_DEBUG
		VO_CORE_TRACE("Initialized engine logger");
		VO_CORE_TRACE("Creating a graphics engine");
#endif
		m_Width = 640;
		m_Height = 480;

		InitializeWindow();
		CreateVulkanInstance();

		SetupDevice();
	}


	Engine::~Engine()
	{
#ifdef VO_DEBUG
		VO_CORE_TRACE("Destroying graphics engine");
#endif

		//destroy device
		m_Device.destroy();


#ifdef VO_DEBUG
		//destroy debug messanger
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessanger, nullptr, m_DispatchLoaderDY);
#endif

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
			VO_CORE_TRACE("Created window successfully");
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

		//create debug messanger
#ifdef VO_DEBUG
		m_DebugMessanger = vkInit::CreateDebugMessanger(m_Instance, m_DispatchLoaderDY);
#endif

	}
	void Engine::SetupDevice()
	{
		m_PhysicalDevice = vkInit::GetPhysicalDevice(m_Instance);
		m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice);
		m_GraphicsQueue = vkInit::GetQueue(m_PhysicalDevice, m_Device);
	}
}