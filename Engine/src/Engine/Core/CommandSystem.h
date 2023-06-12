#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
	class ENGINE_API CommandSystem
	{
	public:
		static void RunCommand(const CString& Command);
		static void RunCommandAsync(const CString& Command);

		static void RunSystemCommand(const CString& Command);
		static void RunSystemCommandAsync(const CString& Command);
	};
}