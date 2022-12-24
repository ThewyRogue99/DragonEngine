#include "depch.h"
#include "AudioEngine.h"

namespace Engine
{
	static Ref<AudioDevice> ActiveAudioDevice = nullptr;

	Ref<const AudioDevice> AudioEngine::GetActiveAudioDevice()
	{
		return ActiveAudioDevice;
	}
}