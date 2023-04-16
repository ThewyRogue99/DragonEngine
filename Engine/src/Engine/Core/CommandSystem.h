#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
	class ENGINE_API CommandSystem
	{
	public:
		static void RunCommand(const CString& Command);
	};
}