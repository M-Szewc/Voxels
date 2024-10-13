#include "vopch.h"
#include "Platform/Windows/WindowsWindow.h"

namespace VoxelEngine {
	
	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		VO_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}
	
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return false;
	}

	void WindowsWindow::SetTitle(std::string title)
	{
		glfwSetWindowTitle(m_Window, title.c_str());
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

#ifdef VO_DEBUG
		VO_CORE_TRACE("Creating windo {0} ({1}, {2})", props.Title, props.Width, props.Height);
#endif

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();

			//no default rendering clinent - we will hook up vulkan later
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			VO_CORE_ASSERT(success, "Could not initialize GLFW");

			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;

		}

		if (m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr)){
#ifdef VO_DEBUG
			VO_CORE_TRACE("Created window successfully");
#endif

		}
		else {
#ifdef VO_DEBUG
			VO_CORE_ERROR("Failed to create window");
#endif
		}

		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(m_Data.VSync);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		
			data.Width = width;
			data.Height = height;
			
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int modes)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);


			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

	}
	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}
}