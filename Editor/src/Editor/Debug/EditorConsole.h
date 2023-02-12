#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Debug/Log.h"

#include <vector>

namespace Engine
{
	class EditorConsole : public Console
	{
	public:
		EditorConsole() : Console() { }

		virtual void OnAttach();

		virtual void OnDetach();

		const std::vector<Logger::LogData>& GetLogs() { return Logs; }

	private:
		void OnLog(const Logger::LogData& Data);

		std::vector<Logger::LogData> Logs = { };
	};
}