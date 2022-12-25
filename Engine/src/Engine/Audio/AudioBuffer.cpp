#include "depch.h"
#include "AudioBuffer.h"

#include <inttypes.h>
#include <AL/al.h>
#include <AL/alext.h>

namespace Engine
{
	Ref<AudioBuffer> AudioBuffer::Create(AudioBufferData& Data)
	{
		Ref<AudioBuffer> result = CreateRef<AudioBuffer>(phold{ 0 });
		result->SetAudioEffect(Data);

		return result;
	}

	bool AudioBuffer::SetAudioEffect(AudioBufferData& BuffData)
	{
		if (BuffData.Data && BuffData.DataSize > 0)
		{
			RemoveAudioEffect();

			ALenum err, format = AL_NONE;
			ALuint buffer = 0;

			/* Get the sound format, and figure out the OpenAL format */
			switch (BuffData.Channels)
			{
			case 1:
			{
				if (BuffData.SampleSize == sizeof(float))
					format = AL_FORMAT_MONO_FLOAT32;
				else if (BuffData.SampleSize == sizeof(short))
					format = AL_FORMAT_MONO16;

			} break;
			case 2:
			{
				if (BuffData.SampleSize == sizeof(float))
					format = AL_FORMAT_STEREO_FLOAT32;
				else if (BuffData.SampleSize == sizeof(short))
					format = AL_FORMAT_STEREO16;

			} break;
			default:
				DE_CORE_ERROR("Unsupported audio format");
				return false;
			}

			if (format == AL_NONE)
				return false;

			alGenBuffers(1, &buffer);
			alBufferData(buffer, format, BuffData.Data, BuffData.DataSize, BuffData.SampleRate);

			/* Check if an error occured, and clean up if so. */
			err = alGetError();
			if (err != AL_NO_ERROR)
			{
				DE_CORE_ERROR("OpenAL Error: %s\n", alGetString(err));
				if (buffer && alIsBuffer(buffer))
					alDeleteBuffers(1, &buffer);

				return false;
			}

			AudioEffectBuffer = buffer;  // add to the list of known buffers

			return true;
		}

		return false;
	}

	bool AudioBuffer::RemoveAudioEffect()
	{
		if (AudioEffectBuffer)
		{
			alDeleteBuffers(1, &AudioEffectBuffer);
			return true;
		}

		return false;
	}

	AudioBuffer::~AudioBuffer()
	{
		RemoveAudioEffect();
	}
}