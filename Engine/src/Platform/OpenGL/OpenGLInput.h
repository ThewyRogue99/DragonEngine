#pragma once

#include "Engine/Core/Input.h"

namespace Engine
{
	class OpenGLInput : public Input
	{
	protected:
		ENGINE_API bool IsKeyPressedImpl(KeyInput KeyCode) override;
		ENGINE_API bool IsMouseButtonPressedImpl(MouseButtonInput Button) override;

		ENGINE_API std::pair<float, float> GetMousePositionImpl() override;
		ENGINE_API float GetMouseXImpl() override;
		ENGINE_API float GetMouseYImpl() override;
	};
}
