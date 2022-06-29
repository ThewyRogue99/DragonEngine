#pragma once

#include "Engine/Types/Types.h"
#include <sstream>
#include <functional>

#include "Engine/Core/Core.h"

namespace Engine
{
	enum class EventType : unsigned int
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const wchar_t* GetName() const override { return TEXT(#type); }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class ENGINE_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const wchar_t* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual CString ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory Category)
		{
			return GetCategoryFlags() & Category;
		}

		inline bool IsHandled()
		{
			return bIsHandled;
		}

		inline void Handle()
		{
			bIsHandled = true;
		}

	protected:
		bool bIsHandled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& event) : DispatchEvent(event) { }

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (!DispatchEvent.bIsHandled && DispatchEvent.GetEventType() == T::GetStaticType())
			{
				DispatchEvent.bIsHandled |= func(*(T*)&DispatchEvent);
				return true;
			}
			return false;
		}

	private:
		Event& DispatchEvent;
	};
}