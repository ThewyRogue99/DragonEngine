#pragma once

#include "Engine/Core/Core.h"

#include <vector>

namespace Engine
{
	struct AudioBufferData
	{
		int Channels = 0;
		int SampleSize = 0;
		uint32_t SampleRate = 0;

		const void* Data = nullptr;
		size_t DataSize = 0;
	};

	class AudioBuffer
	{
	private:
		struct phold;

	public:
		bool SetAudioEffect(AudioBufferData& BuffData);
		bool RemoveAudioEffect();

		explicit AudioBuffer(const phold&) : AudioBuffer() { }

		friend class AudioSource;
		friend class AudioEngine;

	private:
		AudioBuffer() = default;
		AudioBuffer(const AudioBuffer&) = default;

		const AudioBuffer& operator =(const AudioBuffer& other)
		{
			AudioEffectBuffer = other.AudioEffectBuffer;
		}

		struct phold {
			explicit phold(int) {}
		};

	private:
		unsigned int AudioEffectBuffer = 0;
	};
}