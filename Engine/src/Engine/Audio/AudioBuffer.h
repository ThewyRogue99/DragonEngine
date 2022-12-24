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
	public:
		static AudioBuffer* Get();

		bool AddAudioEffect(AudioBufferData& BuffData);
		bool RemoveAudioEffect(const unsigned int& buffer);

	private:
		AudioBuffer();
		~AudioBuffer();

		std::vector<unsigned int> AudioEffectBuffers;
	};
}