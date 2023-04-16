#include "depch.h"
#include "AudioBuffer.h"

#include <inttypes.h>
#include <AL/al.h>
#include <AL/alext.h>

namespace Engine
{
	AudioBuffer::AudioBuffer(const phold&) : AudioBuffer()
	{

	}

	AudioBuffer::~AudioBuffer()
	{
		RemoveAudioEffect();
	}

	Ref<AudioBuffer> AudioBuffer::Create()
	{
		return CreateRef<AudioBuffer>(AudioBuffer::phold{ 0 });
	}

	bool AudioBuffer::SetAudioEffect(Ref<AudioBufferData> BuffData)
	{
		if (BuffData && BuffData->Data && BuffData->DataSize > 0)
		{
			RemoveAudioEffect();

			ALenum err, format = AL_NONE;
			ALuint buffer = 0;

			/* Get the sound format, and figure out the OpenAL format */
			switch (BuffData->Channels)
			{
				case 1:
				{
					if (BuffData->SampleSize == sizeof(float))
						format = AL_FORMAT_MONO_FLOAT32;
					else if (BuffData->SampleSize == sizeof(short))
						format = AL_FORMAT_MONO16;
				} break;
				case 2:
				{
					if (BuffData->SampleSize == sizeof(float))
						format = AL_FORMAT_STEREO_FLOAT32;
					else if (BuffData->SampleSize == sizeof(short))
						format = AL_FORMAT_STEREO16;
				} break;
				default:
					DE_ERROR(AudioBuffer, "Unsupported audio format");
					return false;
			}

			alGenBuffers(1, &buffer);
			alBufferData(buffer, format, BuffData->Data, (ALsizei)BuffData->DataSize, (ALsizei)BuffData->SampleRate);

			/* Check if an error occured, and clean up if so. */
			err = alGetError();
			if (err != AL_NO_ERROR)
			{
				DE_ERROR(AudioBuffer, "OpenAL Error: %s\n", alGetString(err));
				if (buffer && alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);

				return false;
			}

			AudioEffectBuffer = buffer;

			return true;
		}

		return false;
	}

	bool AudioBuffer::RemoveAudioEffect()
	{
		if (AudioEffectBuffer)
		{
			alDeleteBuffers(1, &AudioEffectBuffer);
			AudioEffectBuffer = 0;

			return true;
		}

		return false;
	}

	unsigned int AudioBuffer::GetBufferID() const
	{
		return AudioEffectBuffer;
	}

	AudioBufferData::AudioBufferData(const phold&) : AudioBufferData()
	{

	}

	AudioBufferData::~AudioBufferData()
	{
		if (Data)
		{
			delete Data;
			Data = nullptr;
			DataSize = 0;
		}
	}

	Ref<AudioBufferData> AudioBufferData::Create(int Channels, int SampleSize, uint32_t SampleRate)
	{
		Ref<AudioBufferData> Result = CreateRef<AudioBufferData>(phold{ 0 });
		Result->Channels = Channels;
		Result->SampleSize = SampleSize;
		Result->SampleRate = SampleRate;

		return Result;
	}

	void AudioBufferData::SetData(const void* Src, size_t Size)
	{
		Data = new uint8_t[Size];
		memcpy(Data, Src, Size);

		DataSize = Size;
	}
}