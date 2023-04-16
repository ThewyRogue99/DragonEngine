#pragma once
#include "depch.h"

#include "Core.h"
#include "InputType.h"

namespace Engine
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyInput KeyCode) { return Instance->IsKeyPressedImpl(KeyCode); }
		static bool IsMouseButtonPressed(MouseButtonInput Button) { return Instance->IsMouseButtonPressedImpl(Button); }

		static std::pair<float, float> GetMousePosition() { return Instance->GetMousePositionImpl(); }
		static float GetMouseX() { return Instance->GetMouseXImpl(); }
		static float GetMouseY() { return Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(KeyInput KeyCode) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseButtonInput Button) = 0;

		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		ENGINE_API static Input* Instance;
	};
}