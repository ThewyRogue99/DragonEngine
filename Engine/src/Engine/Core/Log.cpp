#include "depch.h"
#include "Log.h"

namespace Engine
{
	Ref<CoreLogger> Log::s_CoreLogger = nullptr;

	Ref<ClientLogger> Log::s_ClientLogger = nullptr;

	Ref<Console> Log::DebugConsole = nullptr;

	void Log::SetConsole(Ref<Console> NewConsole)
	{
		if (DebugConsole)
			DebugConsole->OnDetach();

		if (NewConsole)
		{
			DebugConsole = NewConsole;
			DebugConsole->OnAttach();

			s_CoreLogger = DebugConsole->GetCoreLogger();
			s_ClientLogger = DebugConsole->GetClientLogger();
		}
	}
}