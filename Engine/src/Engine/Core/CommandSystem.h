#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
	class CommandSystem
	{
	public:
		static void RunCommand(const std::string& Command);
	};
}