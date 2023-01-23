#include "depch.h"
#include "AudioBuffer.h"

#include <inttypes.h>
#include <AL/al.h>
#include <AL/alext.h>

namespace Engine
{
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
				DE_ERROR(AudioBuffer, "Unsupported audio format");
				return false;
			}

			if (format == AL_NONE)
				return false;

			alGenBuffers(1, &buffer);
			alBufferData(buffer, format, BuffData.Data, (ALsizei)BuffData.DataSize, (ALsizei)BuffData.SampleRate);

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
			return true;
		}

		return false;
	}
}