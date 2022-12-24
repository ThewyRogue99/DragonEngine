#include "depch.h"
#include "AudioDevice.h"

#include "AL/al.h"

namespace Engine
{
	AudioDevice::AudioDevice()
	{
		Device = alcOpenDevice(nullptr); // nullptr = get default device
		DE_CORE_ASSERT(Device, "Failed to get audio device");

		Context = alcCreateContext(Device, nullptr);
		DE_CORE_ASSERT(Context, "Failed to set audio context");

		DE_CORE_ASSERT(alcMakeContextCurrent(Context), "Failed to make context current");

		const ALCchar* name = nullptr;
		if (alcIsExtensionPresent(Device, "ALC_ENUMERATE_ALL_EXT"))
			name = alcGetString(Device, ALC_ALL_DEVICES_SPECIFIER);
		if (!name || alcGetError(Device) != AL_NO_ERROR)
			name = alcGetString(Device, ALC_DEVICE_SPECIFIER);
		
		DE_CORE_INFO("Using audio source: {0}", name);
	}

	AudioDevice::~AudioDevice()
	{
		DE_CORE_ASSERT(alcMakeContextCurrent(nullptr), "Failed to set context to nullptr");

		alcDestroyContext(Context);
		DE_CORE_ASSERT(!Context, "Failed to destroy context");

		DE_CORE_ASSERT(alcCloseDevice(Device), "Failed to close sound device");
	}
}