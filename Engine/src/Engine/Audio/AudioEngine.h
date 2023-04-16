#pragma once

#include "AudioDevice.h"
#include "AudioSource.h"
#include "AudioBuffer.h"

#include <glm/glm.hpp>

#include "Engine/Core/Core.h"

namespace Engine
{
	class ENGINE_API AudioEngine
	{
	public:
		static void Init();
		static void Shutdown();

		static Ref<AudioSource> CreateAudioSource(
			glm::vec3 Position = glm::vec3(0.f),
			float Pitch = 1.f,
			float Gain = 1.f
		);

		static const Ref<AudioDevice> GetActiveAudioDevice();

		static bool IsPlaying(Ref<AudioSource> Source);

		static void PlayAudio(Ref<AudioSource> Source);

		static void StopAudio(Ref<AudioSource> Source);
		static void StopAllAudio();
	};
}