#include "depch.h"
#include "ExceptionHandler.h"

namespace Engine
{
	std::unordered_map<std::type_index, void*> ExceptionHandler::DispatcherMap = { };
}