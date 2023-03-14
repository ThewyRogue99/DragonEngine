#pragma once

#include "Engine/Core/Core.h"

#include <vector>

namespace Engine
{
	class AudioBufferData
	{
	private:
		struct phold;

	public:
		explicit AudioBufferData(const phold&) : AudioBufferData() { }

		~AudioBufferData();

		static Ref<AudioBufferData> Create(int Channels, int SampleSize, uint32_t SampleRate);

		void SetData(const void* Src, size_t Size);

		friend class AudioBuffer;

	private:
		AudioBufferData() = default;

		struct phold {
			explicit phold(int) {}
		};

	private:
		int Channels = 0;
		int SampleSize = 0;
		uint32_t SampleRate = 0;

		void* Data = nullptr;
		size_t DataSize = 0;
	};

	class AudioBuffer
	{
	private:
		struct phold;

	public:
		~AudioBuffer();

		static Ref<AudioBuffer> Create();

		bool SetAudioEffect(Ref<AudioBufferData> BuffData);
		bool RemoveAudioEffect();

		unsigned int GetBufferID() const { return AudioEffectBuffer; }

		explicit AudioBuffer(const phold&) : AudioBuffer() { }

	private:
		AudioBuffer() = default;
		AudioBuffer(const AudioBuffer&) = default;

		struct phold {
			explicit phold(int) {}
		};

	private:
		unsigned int AudioEffectBuffer = 0;
	};
}