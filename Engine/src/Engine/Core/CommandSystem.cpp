#include "depch.h"
#include "CommandSystem.h"

#include "Engine/Debug/Log.h"

#include <functional>
#include <unordered_map>
#include <vector>

#define ON_COMMAND_DEF CommandMapType CommandMap =

#define ON_COMMAND(COMMAND, ArgListName, ...) \
{ COMMAND, [](std::vector<CString> ArgListName) __VA_ARGS__ }

namespace Engine
{
	using CommandMapType =
		std::unordered_map<CString, std::function<void(std::vector<CString>)>>;

	ON_COMMAND_DEF
	{
		ON_COMMAND("log", Args,
		{
			std::stringstream ss;
			for (auto& it : Args)
				ss << it << ' ';

			DE_LOG(CommandSystem, "{0}", ss.str());
		})
	};

	void CommandSystem::RunCommand(const CString& Command)
	{
		std::istringstream ss(Command);

		std::vector<CString> Tokens = std::vector<CString>(
			std::istream_iterator<CString>(ss),
			std::istream_iterator<CString>()
		);

		CString CommandStr = Tokens[0];

		auto it = CommandMap.find(CommandStr);
		if (it != CommandMap.end())
		{
			Tokens.erase(Tokens.begin());

			(*it).second(Tokens);
		}
		else
		{
			DE_ERROR(CommandSystem, "Command \"{0}\" is not specified!", CommandStr);
		}
	}
}