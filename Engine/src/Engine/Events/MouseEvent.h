#pragma once

#include "Engine/Core/Core.h"
#include "Event.h"
#include "Engine/Core/Input.h"

namespace Engine
{
	class ENGINE_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: MouseX(x), MouseY(y) { }

		inline float GetX() const { return MouseX; }
		inline float GetY() const { return MouseY; }

		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("MouseMovedEvent: [ X: ")
				<< MouseX << TEXT(", Y: ") << MouseY << TEXT(" ]");

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float MouseX, MouseY;
	};

	class ENGINE_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: xOffset(xOffset), yOffset(yOffset) { }

		inline float GetXOffset() const { return xOffset; }
		inline float GetYOffset() const { return yOffset; }

		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("MouseScrolledEvent: [ X Offset: ")
				<< xOffset << TEXT(", Y Offset: ") << yOffset << TEXT(" ]");

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float xOffset, yOffset;
	};

	class ENGINE_API MouseButtonEvent : public Event
	{
	public:
		inline MouseButtonInput GetMouseButton() const { return Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouseButton | EventCategoryInput)

	protected:
		MouseButtonEvent(MouseButtonInput Button) : Button(Button) { }

		MouseButtonInput Button;
	};

	class ENGINE_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(MouseButtonInput Button) : MouseButtonEvent(Button) { }


		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("MouseButtonPressedEvent: [ Button: ") << (int)Button << TEXT(" ]");

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class ENGINE_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButtonInput Button) : MouseButtonEvent(Button) { }


		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("MouseButtonReleasedEvent: [ Button: ") << (int)Button << TEXT(" ]");

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}