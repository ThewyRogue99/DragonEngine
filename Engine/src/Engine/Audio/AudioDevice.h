#pragma once

#include <AL/alc.h>

namespace Engine
{
	class AudioDevice
	{
	public:
		friend class AudioEngine;

		void Basic();

	private:
		AudioDevice();
		~AudioDevice();

		ALCdevice* Device;
		ALCcontext* Context;
	};
}