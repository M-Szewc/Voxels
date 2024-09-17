#include "vopch.h"

#include "Engine.h"

#include "VulkanInstance.h"
#include "VulkanLogger.h"
#include "VulkanDevice.h"
#include "Swapchain.h"

namespace Game {
	Engine::Engine() {
		// initialize loggers
		Log::Init();

		VO_CORE_FATAL("hello");

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

		//destroy image views
		for (vkUtil::SwapChainFrame frame : m_SwapchainFrames) {
			m_Device.destroyImageView(frame.ImageView);
		}

		//destroy device
		m_Device.destroySwapchainKHR(m_Swapchain);
		m_Device.destroy();
		//destroy surface
		m_Instance.destroySurfaceKHR(m_Surface);

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

		// try to create a window
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

	// create and setup Vulkan instance
	void Engine::CreateVulkanInstance()
	{
		m_Instance = vkInit::MakeInstance(m_ApplicationName);
		m_DispatchLoaderDY = vk::DispatchLoaderDynamic(m_Instance, vkGetInstanceProcAddr);

		//create debug messanger
#ifdef VO_DEBUG
		m_DebugMessanger = vkInit::CreateDebugMessanger(m_Instance, m_DispatchLoaderDY);
#endif

		VkSurfaceKHR c_style_surface;
		if (glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &c_style_surface) != VK_SUCCESS) {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to abstract the glfw surface for Vulkan");
#endif

		}
		else {
#ifdef VO_DEBUG
			VO_CORE_TRACE("Successfully abstracted the glfw surface for Vulkan");
#endif

		}
		m_Surface = c_style_surface;
	}

	// setup device - get physical device and abstract it
	void Engine::SetupDevice()
	{
		m_PhysicalDevice = vkInit::GetPhysicalDevice(m_Instance);
		m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface);
		std::array<vk::Queue, 2> queues = vkInit::GetQueue(m_PhysicalDevice, m_Device, m_Surface);
		m_GraphicsQueue = queues[0];
		m_PresentQueue = queues[1];

		// create swapchain
		vkInit::SwapChainBundle bundle = vkInit::CreateSwapchain(m_Device, m_PhysicalDevice, m_Surface, m_Width, m_Height);
		m_Swapchain = bundle.Swapchain;
		m_SwapchainFrames = bundle.Frames;
		m_SwapchainFormat = bundle.Format;
		m_SwapchainExtent = bundle.Extent;
	}
}