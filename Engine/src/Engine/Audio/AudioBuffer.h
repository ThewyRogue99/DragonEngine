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
		static Ref<AudioBuffer> Create(AudioBufferData& Data = AudioBufferData());

		bool SetAudioEffect(AudioBufferData& BuffData);
		bool RemoveAudioEffect();

		explicit AudioBuffer(const phold&) : AudioBuffer() { }

		~AudioBuffer();

		friend class AudioSource;

		AudioBuffer(const AudioBuffer&) = delete;
		const AudioBuffer& operator =(const AudioBuffer&) = delete;

	private:
		AudioBuffer() = default;

		struct phold {
			explicit phold(int) {}
		};

	private:
		unsigned int AudioEffectBuffer = 0;
	};
}