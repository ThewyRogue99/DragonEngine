#include "EditorConsole.h"

#include <spdlog/sinks/ostream_sink.h>

namespace Engine
{
	void EditorConsole::OnAttach()
	{
		auto sink = CreateRef<spdlog::sinks::ostream_sink_mt>(ConsoleStream);
		sink->set_pattern("%^[%T][%l] %n: %v%$");

		CoreLogger = CreateRef<spdlog::logger>("Engine", sink);
		CoreLogger->set_level(spdlog::level::trace);
		CoreLogger->flush_on(spdlog::level::trace);

		std::string name = TypeUtils::FromUTF16(AppName);
		ClientLogger = CreateRef<spdlog::logger>(name.c_str(), sink);
		ClientLogger->set_level(spdlog::level::trace);
		ClientLogger->flush_on(spdlog::level::trace);
	}

	void EditorConsole::OnDetach()
	{
		CoreLogger = nullptr;
		ClientLogger = nullptr;
	}

	Ref<spdlog::logger> EditorConsole::GetCoreLogger()
	{
		return CoreLogger;
	}

	Ref<spdlog::logger> EditorConsole::GetClientLogger()
	{
		return ClientLogger;
	}
}