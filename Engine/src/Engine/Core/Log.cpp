#include "depch.h"
#include "Log.h"

namespace Engine
{
	Ref<Console> Log::DebugConsole = nullptr;

	void Log::SetConsole(Ref<Console> NewConsole)
	{
		if (DebugConsole)
			DebugConsole->OnDetach();

		if (NewConsole)
		{
			DebugConsole = NewConsole;
			DebugConsole->OnAttach();
		}
	}
}