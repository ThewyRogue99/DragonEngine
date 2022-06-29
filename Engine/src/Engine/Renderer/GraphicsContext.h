#pragma once

#include "Engine/Core/Core.h"

namespace Engine
{
	class ENGINE_API GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}