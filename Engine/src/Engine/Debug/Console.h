#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace Engine
{
	class Console
	{
	public:
		Console(const CString& AppName = TEXT("App")) : AppName(AppName) { }

		virtual void OnAttach() = 0;

		virtual void OnDetach() = 0;

		friend class Log;

	protected:
		CString AppName;

		virtual Ref<spdlog::logger> GetCoreLogger() = 0;

		virtual Ref<spdlog::logger> GetClientLogger() = 0;
	};
}