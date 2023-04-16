#include "depch.h"
#include "AudioSource.h"

#include <AL/al.h>

namespace Engine
{
	AudioSource::AudioSource()
	{
		alGenSources(1, &Source);
	}

	void AudioSource::Destroy()
	{
		if (Source)
		{
			alDeleteSources(1, &Source);
			Source = 0;
		}

		if (Buffer)
		{
			Buffer->RemoveAudioEffect();
			Buffer = nullptr;
		}
	}

	AudioSource::~AudioSource()
	{
		Destroy();
	}

	void AudioSource::SetBuffer(Ref<AudioBuffer> Buff)
	{
		Buffer = Buff;
		alSourcei(Source, AL_BUFFER, Buffer->GetBufferID());
	}

	void AudioSource::SetPitch(float pitch)
	{
		Pitch = pitch;
		alSourcef(Source, AL_PITCH, Pitch);
	}

	void AudioSource::SetGain(float gain)
	{
		Gain = gain;
		alSourcef(Source, AL_GAIN, Gain);
	}

	void AudioSource::SetPosition(const glm::vec3& position)
	{
		Position = position;
		alSource3f(Source, AL_POSITION, Position.x, Position.y, Position.z);
	}

	void AudioSource::SetVelocity(const glm::vec3& velocity)
	{
		Velocity = velocity;
		alSource3f(Source, AL_VELOCITY, Velocity.x, Velocity.y, Velocity.z);
	}

	void AudioSource::SetLoopSound(bool loop)
	{
		LoopSound = loop;
		alSourcei(Source, AL_LOOPING, LoopSound);
	}
}