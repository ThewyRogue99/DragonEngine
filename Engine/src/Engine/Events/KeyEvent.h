#pragma once

#include "Event.h"
#include "Engine/Core/Input.h"

namespace Engine
{
	class ENGINE_API KeyEvent : public Event
	{
	public:
		inline KeyInput GetKey() const { return Key; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(KeyInput Key) : Key(Key) { }

		KeyInput Key;
	};

	class ENGINE_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(KeyInput Key, int RepeatCount)
			: KeyEvent(Key), RepeatCount(RepeatCount) { }

		inline int GetRepeatCount() const { return RepeatCount; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: [ Key Code: "
				<< (int)Key << ", Repeats: " << RepeatCount << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int RepeatCount;
	};

	class ENGINE_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(KeyInput Key) : KeyEvent(Key) { }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: [ Key Code: " << (int)Key << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class ENGINE_API KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(unsigned int Character) : Character(Character) { }

		inline unsigned int GetCharacter() { return Character; }

		virtual std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: [ Character: " << Character << " ]";

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	private:
		unsigned int Character;
	};
}