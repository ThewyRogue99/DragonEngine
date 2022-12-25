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

		static Ref<const AudioDevice> GetActiveAudioDevice();

		static void PlayAudio(Ref<AudioSource> Source);

		static void StopAudio(Ref<AudioSource> Source);
	};
}