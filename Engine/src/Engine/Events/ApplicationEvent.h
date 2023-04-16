#pragma once

#include "Event.h"

namespace Engine
{
	class ENGINE_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int Width, unsigned int Height)
			: Width(Width), Height(Height) { }

		inline unsigned int GetWidth() const { return Width; }
		inline unsigned int GetHeight() const { return Height; }

		WString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("WindowResizeEvent: [ Width: ") << Width << TEXT(", Height: ") << Height << TEXT(" ]");
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		unsigned int Width, Height;
	};

	class ENGINE_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() { }

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ENGINE_API AppTickEvent : public Event
	{
	public:
		AppTickEvent() { }

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ENGINE_API AppUpdateEvent : public Event
	{
	public:
		AppUpdateEvent() { }

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	class ENGINE_API AppRenderEvent : public Event
	{
	public:
		AppRenderEvent() { }

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}