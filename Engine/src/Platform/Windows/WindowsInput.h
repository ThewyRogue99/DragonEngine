#pragma once

#include "Engine/Input.h"

namespace Engine
{
	class WindowsInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(KeyInput KeyCode) override;
		bool IsMouseButtonPressedImpl(MouseButtonInput Button) override;

		std::pair<float, float> GetMousePositionImpl() override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;
	};
}
