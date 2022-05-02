#pragma once

#include "Core.h"

namespace Engine
{
	class ENGINE_API Timestep
	{
	public:
		Timestep(float Time = 0.f) : Time(Time) { }
		
		operator float() const { return Time; }

		inline float GetSeconds() const { return Time; }
		inline float GetMilliseconds() const { return Time * 1000.f; }

	private:
		float Time;
	};
}