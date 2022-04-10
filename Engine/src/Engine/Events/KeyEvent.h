#pragma once

#include "Event.h"

namespace Engine
{
	class ENGINE_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode() const { return KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int KeyCode) : KeyCode(KeyCode) { }

		int KeyCode;
	};

	class ENGINE_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int KeyCode, int RepeatCount)
			: KeyEvent(KeyCode), RepeatCount(RepeatCount) { }

		inline int GetRepeatCount() const { return RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: [ Key Code: "
				<< KeyCode << ", Repeats: " << RepeatCount << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int RepeatCount;
	};

	class ENGINE_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int KeyCode) : KeyEvent(KeyCode) { }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: [ Key Code: " << KeyCode << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}