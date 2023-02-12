#pragma once

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	class EditorTool
	{
	public:
		static void BeginPlay();
		static void EndPlay();

		static void BeginSimulation();
		static void EndSimulation();

		static bool IsPlaying();
		static bool IsSimulating();

		static CallbackDispatcher<void>::CallbackHandle OnBeginPlay();
		static CallbackDispatcher<void>::CallbackHandle OnEndPlay();
	};
}