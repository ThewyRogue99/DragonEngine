#include "depch.h"
#include "EditorConsole.h"

namespace Engine
{
	void EditorConsole::OnAttach()
	{
		ConsoleLogger = CreateRef<Logger>("Editor");
		ConsoleLogger->OnLog().AddCallback(BIND_CLASS_FN(OnLog));

		DE_INFO(EditorConsole, "Created Editor Console");
	}

	void EditorConsole::OnDetach()
	{
		ConsoleLogger = nullptr;
	}

	void EditorConsole::OnLog(const Logger::LogData& Data)
	{
		Logs.push_back(Data);
	}
}