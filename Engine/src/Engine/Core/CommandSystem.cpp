#include "depch.h"
#include "CommandSystem.h"

#include "Engine/Debug/Log.h"

#include <functional>
#include <unordered_map>
#include <array>
#include <vector>
#include <thread>

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
		}),
		ON_COMMAND("compile", Args,
		{
			
		}),
	};

	void CommandSystem::RunCommand(const CString& Command)
	{
		std::istringstream ss(Command);

		std::vector<CString> Tokens = std::vector<CString>(
			std::istream_iterator<CString>(ss),
			std::istream_iterator<CString>()
		);

		CString CommandSpec = Tokens[0];

		std::vector<CString> CommandTokens(Tokens.begin() + 1, Tokens.end());
		Tokens.clear();

		CString CommandStr = CommandTokens[0];

		if (CommandSpec == "engine")
		{
			auto it = CommandMap.find(CommandStr);
			if (it != CommandMap.end())
			{
				(*it).second(std::vector<CString>(CommandTokens.begin() + 1, CommandTokens.end()));
			}
			else
			{
				DE_ERROR(CommandSystem, "Command '{0}' is not specified!", CommandStr);
			}
		}
		else if (CommandSpec == "shell")
		{
			CString FullCommandStr;

			for (auto& token : CommandTokens) {
				FullCommandStr += ' ' + token;
			}

			RunSystemCommand(FullCommandStr);
		}
		else
		{
			DE_ERROR(CommandSystem, "Use 'shell' or 'engine' before a command");
		}
	}

	void CommandSystem::RunCommandAsync(const CString& Command)
	{
		std::thread t(&CommandSystem::RunCommand, Command);

		t.detach();
	}

	void CommandSystem::RunSystemCommand(const CString& Command)
	{
		std::array<char, 128> buffer;

		FILE* pipe = _popen(Command.c_str(), "r");
		if (!pipe) {
			throw std::runtime_error("_popen() failed!");
		}

		ConsoleStream& cs = Log::GetConsole()->GetStream();

		cs << ConsoleStream::BeginLog("Shell") << LogLevel::Command;

		while (fgets(buffer.data(), (int)buffer.size(), pipe) != nullptr) {
			cs << buffer.data();
		}

		cs << ConsoleStream::EndLog();

		_pclose(pipe);
	}

	void CommandSystem::RunSystemCommandAsync(const CString& Command)
	{
		std::thread t(&CommandSystem::RunSystemCommand, Command);

		t.detach();
	}
}