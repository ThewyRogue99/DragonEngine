#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Core/Log.h"

#include <sstream>

namespace Engine
{
	class EditorConsole : public Console
	{
	public:
		EditorConsole(const CString& AppName = TEXT("App")) : Console(AppName) { }

		virtual void OnAttach();

		virtual void OnDetach();

		std::string GetString() { return ConsoleStream.str(); }

	protected:
		virtual Ref<spdlog::logger> GetCoreLogger();

		virtual Ref<spdlog::logger> GetClientLogger();

	private:
		std::ostringstream ConsoleStream;

		Ref<spdlog::logger> CoreLogger = nullptr;

		Ref<spdlog::logger> ClientLogger = nullptr;
	};
}