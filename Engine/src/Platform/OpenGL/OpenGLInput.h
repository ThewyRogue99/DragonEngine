#pragma once

#include "Engine/Core/Input.h"

namespace Engine
{
	class OpenGLInput : public Input
	{
	protected:
		bool IsKeyPressedImpl(KeyInput KeyCode) override;
		bool IsMouseButtonPressedImpl(MouseButtonInput Button) override;

		std::pair<float, float> GetMousePositionImpl() override;
		float GetMouseXImpl() override;
		float GetMouseYImpl() override;
	};
}
