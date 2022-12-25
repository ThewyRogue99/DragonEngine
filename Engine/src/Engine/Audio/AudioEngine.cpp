#include "depch.h"
#include "AudioEngine.h"

#include <future>
#include <AL/al.h>

namespace Engine
{
	static Ref<AudioDevice> ActiveAudioDevice = nullptr;

	void AudioEngine::Init()
	{
		ActiveAudioDevice = AudioDevice::OpenDevice();
	}

	Ref<const AudioDevice> AudioEngine::GetActiveAudioDevice()
	{
		return ActiveAudioDevice;
	}

	void AudioEngine::PlayAudio(Ref<AudioSource> Source)
	{
		std::async(std::launch::async, [Source]() { alSourcePlay(Source->Source); });
	}

	void AudioEngine::StopAudio(Ref<AudioSource> Source)
	{
		alSourceStop(Source->Source);
	}
}