#include "depch.h"
#include "CommandSystem.h"

#include "Engine/Debug/Log.h"

#include <functional>
#include <unordered_map>
#include <vector>

#define ON_COMMAND_DEF CommandMapType CommandMap =

#define ON_COMMAND(COMMAND, ArgListName, ...) \
{ COMMAND, [](std::vector<std::string> ArgListName) __VA_ARGS__ }

namespace Engine
{
	using CommandMapType =
		std::unordered_map<std::string, std::function<void(std::vector<std::string>)>>;

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

	void CommandSystem::RunCommand(const std::string& Command)
	{
		std::istringstream ss(Command);

		std::vector<std::string> Tokens = std::vector<std::string>(
			std::istream_iterator<std::string>(ss),
			std::istream_iterator<std::string>()
		);

		std::string CommandStr = Tokens[0];

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