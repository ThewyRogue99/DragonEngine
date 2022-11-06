#include "depch.h"
#include "EditorConsole.h"

namespace Engine
{
	void EditorConsole::OnAttach()
	{
		m_CoreLogger = CreateRef<CoreLogger>("Engine", this);
		m_ClientLogger = CreateRef<ClientLogger>("App", this);
	}

	void EditorConsole::OnDetach()
	{
		m_CoreLogger = nullptr;
		m_ClientLogger = nullptr;
	}

	Ref<CoreLogger> EditorConsole::GetCoreLogger()
	{
		return m_CoreLogger;
	}

	Ref<ClientLogger> EditorConsole::GetClientLogger()
	{
		return m_ClientLogger;
	}
}