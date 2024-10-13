#pragma once

#include "Events/Event.h"

namespace VoxelEngine {

	class VO_API WindowCloseEvent : public VOEvent
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
		
	class VO_API WindowResizeEvent :public VOEvent
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			:m_Width(width), m_Height(height) {}

		inline unsigned int GetWidth() const { return m_Width; }
		inline unsigned int GetHeight() const { return m_Height; }
	
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	
	private:
		unsigned int m_Width, m_Height;

	};
		
	class VO_API WindowFocusEvent : public VOEvent
	{
	public:
		WindowFocusEvent() {}

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class VO_API WindowLostFocusEvent : public VOEvent
	{
	public:
		WindowLostFocusEvent() {}

		EVENT_CLASS_TYPE(WindowFocus)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class VO_API WindowMovedEvent : public VOEvent
	{
	public:
		WindowMovedEvent() {}

		EVENT_CLASS_TYPE(WindowMoved)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
	
	class VO_API AppTickEvent : public VOEvent
	{
	public:
		AppTickEvent() {}

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
	
	class VO_API AppUpdateEvent : public VOEvent
	{
	public:
		AppUpdateEvent() {}

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class VO_API AppRenderEvent : public VOEvent
	{
	public:
		AppRenderEvent() {}

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}

VO_LOG_FORMATTER(VoxelEngine::WindowCloseEvent)
VO_LOG_FORMATTER(VoxelEngine::WindowResizeEvent)
VO_LOG_FORMATTER(VoxelEngine::WindowFocusEvent)
VO_LOG_FORMATTER(VoxelEngine::WindowLostFocusEvent)
VO_LOG_FORMATTER(VoxelEngine::WindowMovedEvent)
VO_LOG_FORMATTER(VoxelEngine::AppTickEvent)
VO_LOG_FORMATTER(VoxelEngine::AppUpdateEvent)
VO_LOG_FORMATTER(VoxelEngine::AppRenderEvent)