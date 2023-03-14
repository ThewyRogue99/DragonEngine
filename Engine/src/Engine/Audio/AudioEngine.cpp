#include "depch.h"
#include "AudioEngine.h"

#include <vector>

#include <AL/al.h>

#include <algorithm>

namespace Engine
{
	static Ref<AudioDevice> ActiveAudioDevice = nullptr;

	static std::vector<WeakRef<AudioSource>> AudioSourceList = { };

	void AudioEngine::Init()
	{
		DE_LOG(AudioEngine, "Initializing AudioEngine...");

		ActiveAudioDevice = AudioDevice::OpenDevice();

		if (ActiveAudioDevice)
		{
			DE_INFO(AudioEngine, "Successfully initialized AudioEngine");
			DE_INFO(AudioEngine, "Using audio device: {0}", ActiveAudioDevice->GetName().c_str());
		}
	}

	void AudioEngine::Shutdown()
	{
		for (auto& src : AudioSourceList)
		{
			if (!src.expired())
			{
				src.lock()->Destroy();
			}
		}

		AudioSourceList.clear();

		ActiveAudioDevice = nullptr;
	}

	Ref<AudioSource> AudioEngine::CreateAudioSource(glm::vec3 Position, float Pitch, float Gain)
	{
		Ref<AudioSource> result = CreateRef<AudioSource>(AudioSource::phold{ 0 });
		result->SetPitch(Pitch);
		result->SetGain(Gain);
		result->SetPosition(Position);
		result->SetVelocity(glm::vec3(0.f));
		result->SetLoopSound(false);

		AudioSourceList.push_back(result);

		return result;
	}

	const Ref<AudioDevice> AudioEngine::GetActiveAudioDevice()
	{
		return ActiveAudioDevice;
	}

	bool AudioEngine::IsPlaying(Ref<AudioSource> Source)
	{
		if (Source && Source->GetSourceID())
		{
			ALint state;
			alGetSourcei(Source->GetSourceID(), AL_SOURCE_STATE, &state);

			return state == AL_PLAYING;
		}

		return false;
	}

	void AudioEngine::PlayAudio(Ref<AudioSource> Source)
	{
		if (Source)
		{
			Ref<AudioBuffer> Buff = Source->GetBuffer();
			if (Buff && Buff->GetBufferID())
			{
				alSourcePlay(Source->Source);
			}
		}
	}

	void AudioEngine::StopAudio(Ref<AudioSource> Source)
	{
		if (Source)
		{
			Ref<AudioBuffer> Buff = Source->GetBuffer();
			if (Buff && Buff->GetBufferID())
			{
				alSourceStop(Source->GetSourceID());
			}
		}
	}

	void AudioEngine::StopAllAudio()
	{
		DE_WARN(AudioEngine, "Stopping all playing Audio");

		for (auto& src : AudioSourceList)
		{
			if (!src.expired())
			{
				alSourceStop(src.lock()->GetSourceID());
			}
		}
	}
}