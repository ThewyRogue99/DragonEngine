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

		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("KeyPressedEvent: [ Key Code: ")
				<< (int)Key << TEXT(", Repeats: ") << RepeatCount << TEXT(" ]");

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

		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("KeyReleasedEvent: [ Key Code: ") << (int)Key << TEXT(" ]");

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class ENGINE_API KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(unsigned int Character) : Character(Character) { }

		inline unsigned int GetCharacter() { return Character; }

		virtual CString ToString() const override
		{
			std::wstringstream ss;
			ss << TEXT("KeyTypedEvent: [ Character: ") << Character << TEXT(" ]");

			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
		EVENT_CLASS_CATEGORY(EventCategoryInput | EventCategoryKeyboard);

	private:
		unsigned int Character;
	};
}