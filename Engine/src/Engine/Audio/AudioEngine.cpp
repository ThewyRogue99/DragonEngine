#include "depch.h"
#include "AudioEngine.h"

#include <unordered_map>

#include <thread>
#include <AL/al.h>

namespace Engine
{
	static Ref<AudioDevice> ActiveAudioDevice = nullptr;

	static std::unordered_map<unsigned int, unsigned int> PlayingAudioList = { };

	void AudioEngine::Init()
	{
		ActiveAudioDevice = AudioDevice::OpenDevice();
	}

	Ref<const AudioDevice> AudioEngine::GetActiveAudioDevice()
	{
		return ActiveAudioDevice;
	}

	bool AudioEngine::IsPlaying(Ref<AudioSource> Source)
	{
		ALint state;
		alGetSourcei(Source->Source, AL_SOURCE_STATE, &state);

		return state == AL_PLAYING;
	}

	void AudioEngine::PlayAudio(Ref<AudioSource> Source)
	{
		PlayingAudioList[Source->Source] = Source->Source;

		alSourcePlay(Source->Source);
	}

	void AudioEngine::StopAudio(Ref<AudioSource> Source)
	{
		alSourceStop(Source->Source);
		PlayingAudioList.erase(Source->Source);
	}

	void AudioEngine::StopAllAudio()
	{
		for (auto& [src1, src2] : PlayingAudioList)
			alSourceStop(src2);

		PlayingAudioList.clear();
	}
}