#pragma once

#include "Event.h"
#include "Engine/Input.h"

namespace Engine
{
	class ENGINE_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: MouseX(x), MouseY(y) { }

		inline float GetX() const { return MouseX; }
		inline float GetY() const { return MouseY; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: [ X: "
				<< MouseX << ", Y: " << MouseY << " ]";

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

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: [ X Offset: "
				<< xOffset << ", Y Offset: " << yOffset << " ]";

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


		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: [ Button: " << (int)Button << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class ENGINE_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(MouseButtonInput Button) : MouseButtonEvent(Button) { }


		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: [ Button: " << (int)Button << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}