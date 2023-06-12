#include "depch.h"
#include "Console.h"

#include "Log.h"
#include "Engine/Debug/Assert.h"

#include <ctime>
#include <optional>
#include <thread>

namespace Engine
{
	ConsoleStream::ConsoleStream()
	{
		
	}

	ConsoleStream::~ConsoleStream()
	{
		
	}

	ConsoleStream::LogBeginStruct ConsoleStream::BeginLog(const CString& LoggerName)
	{
		std::time_t result = std::time(nullptr);

		char tbuff[128];
		std::strftime(tbuff, 128, "[%T]", std::localtime(&result));

		return { tbuff, LoggerName };
	}

	std::nullptr_t ConsoleStream::EndLog()
	{
		return nullptr;
	}

	ConsoleStream& ConsoleStream::operator<<(const CString& str)
	{
		uint64_t ThreadID = Thread_GetID();

		LogEntry* ActiveLogEntry = Thread_GetActiveLogEntry(ThreadID);

		DE_ASSERT(ActiveLogEntry, "No LogEntry is available on this thread. Write the 'BeginLog' function result to the stream first");

		ActiveLogEntry->LogString += str;

		return *this;
	}

	ConsoleStream& ConsoleStream::operator<<(std::nullptr_t ptr)
	{
		std::lock_guard<std::mutex> lock(LogMutex);

		uint64_t ThreadID = Thread_GetID();

		if (Thread_IsLogging(ThreadID)) {
			EndCurrentEntry();
		}
		else
		{
			LogEntry* ActiveLogEntry = Thread_GetActiveLogEntry(ThreadID);

			DE_ASSERT(ActiveLogEntry, "No active LogEntry is available on this thread. Write the 'BeginLog' function result to the stream first");

			ActiveLogEntry->IsActive = false;
		}

		return *this;
	}

	ConsoleStream& ConsoleStream::operator<<(const LogLevel& level)
	{
		uint64_t ThreadID = Thread_GetID();

		LogEntry* ActiveLogEntry = Thread_GetActiveLogEntry(ThreadID);

		DE_ASSERT(ActiveLogEntry, "No active LogEntry is available on this thread. Write the 'BeginLog' function result to the stream first");

		ActiveLogEntry->Level = level;

		return *this;
	}

	ConsoleStream& ConsoleStream::operator<<(const LogBeginStruct& data)
	{
		std::lock_guard<std::mutex> lock(LogMutex);

		uint64_t ThreadID = Thread_GetID();

		LogEntry* ActiveLogEntry = Thread_GetActiveLogEntry(ThreadID);

		LogEntry* NewEntry = new LogEntry();
		NewEntry->Timestamp = data.Timestamp;
		NewEntry->LoggerName = data.LoggerName;
		NewEntry->ThreadID = ThreadID;
		NewEntry->IsActive = true;

		if (ActiveLogEntry) {
			Thread_SetActiveLogEntry(ThreadID, NewEntry);
		}
		else
		{
			if (CurrentEntry) {
				LogEntryQueue.push_back(NewEntry);
			}
			else
			{
				CurrentEntry = NewEntry;
				LogBuffer.push_back(CurrentEntry);
			}
		}

		return *this;
	}

	bool ConsoleStream::Thread_IsLogging(uint64_t ThreadID)
	{
		if (CurrentEntry)
		{
			return CurrentEntry->ThreadID == ThreadID;
		}
	}

	ConsoleStream::LogEntry* ConsoleStream::Thread_GetActiveLogEntry(uint64_t ThreadID)
	{
		if (CurrentEntry)
		{
			if (Thread_IsLogging(ThreadID))
				return CurrentEntry;

			// Get the active LogEntry from the LogEntryQueue
			for (LogEntry* entry : LogEntryQueue)
			{
				if (entry->ThreadID == ThreadID && entry->IsActive)
					return entry;
			}
		}
		// If there is not a CurrentEntry then no entry is available

		return nullptr;
	}

	uint64_t ConsoleStream::Thread_GetID()
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();

		return std::stoull(ss.str());
	}

	bool ConsoleStream::Thread_SetActiveLogEntry(uint64_t ThreadID, LogEntry* NewEntry)
	{
		if (CurrentEntry && NewEntry)
		{
			if (Thread_IsLogging(ThreadID))
			{
				delete CurrentEntry;

				CurrentEntry = NewEntry;
				LogBuffer.back() = CurrentEntry;

				return true;
			}

			// Get the active LogEntry from the LogEntryQueue
			for (auto& Entry : LogEntryQueue)
			{
				if (Entry->ThreadID == ThreadID && Entry->IsActive)
				{
					delete Entry;
					Entry = NewEntry;

					return true;
				}
			}
		}
		// If there is not a CurrentEntry then no entry is available

		return false;
	}

	void ConsoleStream::EndCurrentEntry()
	{
		if (CurrentEntry)
		{
			CurrentEntry->IsActive = false;
			CurrentEntry = nullptr;

			// Flush LogEntryQueue (if there is an active entry, set it as CurrentEntry then break)

			size_t QueueSize = LogEntryQueue.size();
			for (int i = 0; i < QueueSize; i++)
			{
				LogEntry* entry = LogEntryQueue.front();

				LogBuffer.push_back(entry);
				LogEntryQueue.pop_front();

				if (entry->IsActive)
				{
					CurrentEntry = entry;
					break;
				}
			}
		}
	}

	void Console::_Log(const CString& LoggerName, ConsoleStream::LogLevel Level, const CString& LogString)
	{
		CStream << ConsoleStream::BeginLog(LoggerName) << Level << LogString << ConsoleStream::EndLog();
	}
}