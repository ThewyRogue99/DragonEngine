#pragma once

#include "Engine/Core/Core.h"
#include "AudioDevice.h"
#include "AudioSource.h"
#include "AudioBuffer.h"

#include <glm/glm.hpp>

namespace Engine
{
	class AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<AudioBuffer> CreateAudioBuffer(AudioBufferData& Data);
		static Ref<AudioSource> CreateAudioSource(glm::vec3 Position = glm::vec3(0.f), float Pitch = 1.f, float Gain = 1.f);

		static bool DestroyAudioBuffer(Ref<AudioBuffer> Buffer);
		static bool DestroyAudioSource(Ref<AudioSource> Source);

		static Ref<const AudioDevice> GetActiveAudioDevice();

		static bool IsPlaying(Ref<AudioSource> Source);

		static void PlayAudio(Ref<AudioSource> Source);

		static void StopAudio(Ref<AudioSource> Source);
		static void StopAllAudio();
	};
}