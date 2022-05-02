#pragma once

#include "Core.h"
#include "InputType.h"

namespace Engine
{
	class ENGINE_API Input
	{
	public:
		inline static bool IsKeyPressed(KeyInput KeyCode) { return Instance->IsKeyPressedImpl(KeyCode); }
		inline static bool IsMouseButtonPressed(MouseButtonInput Button) { return Instance->IsMouseButtonPressedImpl(Button); }

		inline static std::pair<float, float> GetMousePosition() { return Instance->GetMousePositionImpl(); }
		inline static float GetMouseX() { return Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return Instance->GetMouseYImpl(); }

	protected:
		virtual bool IsKeyPressedImpl(KeyInput KeyCode) = 0;
		virtual bool IsMouseButtonPressedImpl(MouseButtonInput Button) = 0;

		virtual std::pair<float, float> GetMousePositionImpl() = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;

	private:
		static Input* Instance;
	};
}