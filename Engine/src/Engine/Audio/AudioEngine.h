#pragma once

#include "Engine/Core/Core.h"
#include "AudioDevice.h"

namespace Engine
{
	class AudioEngine
	{
	public:
		static Ref<const AudioDevice> GetActiveAudioDevice();
	};
}