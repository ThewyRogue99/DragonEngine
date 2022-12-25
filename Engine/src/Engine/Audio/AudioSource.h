#pragma once

#include "Engine/Core/Core.h"
#include "AudioBuffer.h"

namespace Engine
{
	class AudioSource
	{
	private:
		struct phold;

	public:
		explicit AudioSource(const phold& hold) : AudioSource() { }
		~AudioSource();

		static Ref<AudioSource> Create(glm::vec3 Position = glm::vec3(0.f), float Pitch = 1.f, float Gain = 1.f);

		void SetBuffer(Ref<AudioBuffer> Buff);

		float GetPitch() const { return Pitch; }
		void SetPitch(float pitch);

		float GetGain() const { return Gain; }
		void SetGain(float gain);

		glm::vec3 GetPosition() const { return Position; }
		void SetPosition(glm::vec3 position);

		glm::vec3 GetVelocity() const { return Velocity; }
		void SetVelocity(glm::vec3 velocity);

		float GetLoopSound() const { return LoopSound; }
		void SetLoopSound(bool loop);

		friend class AudioEngine;

		AudioSource(const AudioSource&) = delete;
		const AudioSource& operator =(const AudioSource&) = delete;

	private:
		AudioSource();

		struct phold {
			explicit phold(int) {}
		};

	private:
		float Pitch = 1.f;
		float Gain = 1.f;

		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Velocity = glm::vec3(0.f);

		bool LoopSound = false;

		unsigned int Source;
		Ref<AudioBuffer> Buffer;
	};
}