#include "depch.h"
#include "AudioEngine.h"

#include <vector>

#include <AL/al.h>

#include <algorithm>

namespace Engine
{
	static Ref<AudioDevice> ActiveAudioDevice = nullptr;

	static std::vector<Ref<AudioBuffer>> AudioBufferList = { };
	static std::vector<Ref<AudioSource>> AudioSourceList = { };

	void AudioEngine::Init()
	{
		DE_LOG(AudioEngine, "Initializing AudioEngine...");

		ActiveAudioDevice = AudioDevice::OpenDevice();

		if (ActiveAudioDevice)
		{
			DE_INFO(AudioEngine, "Successfully initialized AudioEngine");
			DE_INFO(AudioEngine, "Using audio device: {0}", ActiveAudioDevice->DeviceName);
		}
	}

	void AudioEngine::Shutdown()
	{
		for (auto& src : AudioSourceList)
			src->Destroy();

		for (auto& buff : AudioBufferList)
			buff->RemoveAudioEffect();

		AudioSourceList.clear();
		AudioBufferList.clear();

		ActiveAudioDevice = nullptr;
	}

	Ref<AudioBuffer> AudioEngine::CreateAudioBuffer(AudioBufferData& Data)
	{
		Ref<AudioBuffer> result = CreateRef<AudioBuffer>(AudioBuffer::phold{ 0 });
		result->SetAudioEffect(Data);

		AudioBufferList.push_back(result);

		return result;
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

	bool AudioEngine::DestroyAudioBuffer(Ref<AudioBuffer> Buffer)
	{
		auto& it = std::find_if(AudioBufferList.begin(), AudioBufferList.end(), [Buffer](const Ref<AudioBuffer>& Buff)
		{
			return Buff == Buffer;
		});

		if (it != AudioBufferList.end())
		{
			(*it)->RemoveAudioEffect();
			(*it)->AudioEffectBuffer = 0;

			AudioBufferList.erase(it);

			return true;
		}

		return false;
	}

	bool AudioEngine::DestroyAudioSource(Ref<AudioSource> Source)
	{
		auto& it = std::find_if(AudioSourceList.begin(), AudioSourceList.end(), [Source](const Ref<AudioSource>& Src)
		{
			return Src == Source;
		});

		if (it != AudioSourceList.end())
		{
			(*it)->Destroy();
			(*it)->Source = 0;

			AudioSourceList.erase(it);

			return true;
		}

		return false;
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
		if(Source && Source->Buffer->AudioEffectBuffer)
			alSourcePlay(Source->Source);
	}

	void AudioEngine::StopAudio(Ref<AudioSource> Source)
	{
		if (Source && Source->Buffer->AudioEffectBuffer)
			alSourceStop(Source->Source);
	}

	void AudioEngine::StopAllAudio()
	{
		DE_WARN(AudioEngine, "Stopping all playing Audio");

		for (auto& src : AudioSourceList)
			alSourceStop(src->Source);
	}
}