#include "depch.h"
#include "AudioSource.h"

#include <AL/al.h>

namespace Engine
{
	AudioSource::AudioSource()
	{
		alGenSources(1, &Source);
	}

	AudioSource::~AudioSource()
	{
		alDeleteSources(1, &Source);
	}

	Ref<AudioSource> AudioSource::Create(glm::vec3 Position, float Pitch, float Gain)
	{
		Ref<AudioSource> result = CreateRef<AudioSource>(phold{ 0 });
		result->SetPitch(Pitch);
		result->SetGain(Gain);
		result->SetPosition(Position);
		result->SetVelocity(glm::vec3(0.f));
		result->SetLoopSound(false);

		return result;
	}

	void AudioSource::SetBuffer(Ref<AudioBuffer> Buff)
	{
		Buffer = Buff;
		alSourcei(Source, AL_BUFFER, Buffer->AudioEffectBuffer);
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

	void AudioSource::SetPosition(glm::vec3 position)
	{
		Position = position;
		alSource3f(Source, AL_POSITION, Position.x, Position.y, Position.z);
	}

	void AudioSource::SetVelocity(glm::vec3 velocity)
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