#pragma once

#include "Core/Base.h"

namespace VoxelEngine {

	enum class VOEventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum VOEventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static VOEventType GetStaticType() { return VOEventType::##type; }\
								virtual VOEventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class VO_API VOEvent
	{
		friend class VOEventDispatcher;
	public:
		virtual VOEventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(VOEventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false;
	};

	class VOEventDispatcher
	{
		template<typename T>
		using EventFn = std::is_function<bool(T&)>;
	public:
		VOEventDispatcher(VOEvent& event)
			: m_Event(event)
		{

		}

		// function type F is deduced by compiler
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		VOEvent& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const VOEvent& e)
	{
		return os << e.ToString();
	}
}

VO_LOG_FORMATTER(VoxelEngine::VOEvent)