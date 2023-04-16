#pragma once

#include "Engine/Core/Core.h"
#include "AudioBuffer.h"

#include <glm/glm.hpp>

namespace Engine
{
	class AudioSource
	{
	private:
		struct phold;

	public:
		explicit AudioSource(const phold& hold) : AudioSource() { }

		ENGINE_API ~AudioSource();

		unsigned int GetSourceID() const { return Source; }

		ENGINE_API void SetBuffer(Ref<AudioBuffer> Buff);
		Ref<AudioBuffer> GetBuffer() const { return Buffer; }

		float GetPitch() const { return Pitch; }
		ENGINE_API void SetPitch(float pitch);

		float GetGain() const { return Gain; }
		ENGINE_API void SetGain(float gain);

		const glm::vec3& GetPosition() const { return Position; }
		ENGINE_API void SetPosition(const glm::vec3& position);

		const glm::vec3& GetVelocity() const { return Velocity; }
		ENGINE_API void SetVelocity(const glm::vec3& velocity);

		float GetLoopSound() const { return LoopSound; }
		ENGINE_API void SetLoopSound(bool loop);

		friend class AudioEngine;

		AudioSource(const AudioSource&) = delete;
		const AudioSource& operator =(const AudioSource&) = delete;

	private:
		ENGINE_API AudioSource();

		ENGINE_API void Destroy();

		struct phold {
			explicit phold(int) {}
		};

	private:
		float Pitch = 1.f;
		float Gain = 1.f;

		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Velocity = glm::vec3(0.f);

		bool LoopSound = false;

		unsigned int Source = 0;
		Ref<AudioBuffer> Buffer = nullptr;
	};
}