#pragma once

#include "Engine/Core/Core.h"

#include <vector>

namespace Engine
{
	class ENGINE_API AudioBufferData
	{
	private:
		struct phold;

	public:
		explicit AudioBufferData(const phold&);

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
		int Channels;
		int SampleSize;
		uint32_t SampleRate;

		void* Data;
		size_t DataSize;
	};

	class ENGINE_API AudioBuffer
	{
	private:
		struct phold;

	public:
		~AudioBuffer();

		static Ref<AudioBuffer> Create();

		bool SetAudioEffect(Ref<AudioBufferData> BuffData);
		bool RemoveAudioEffect();

		unsigned int GetBufferID() const;

		explicit AudioBuffer(const phold&);

	private:
		AudioBuffer() = default;
		AudioBuffer(const AudioBuffer&) = default;

		struct phold {
			explicit phold(int) {}
		};

	private:
		unsigned int AudioEffectBuffer;
	};
}