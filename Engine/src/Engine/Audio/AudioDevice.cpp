#include "depch.h"
#include "AudioDevice.h"

#include <AL/al.h>
#include <AL/alc.h>

namespace Engine
{
	Ref<AudioDevice> AudioDevice::OpenDevice(const char* DeviceName)
	{
		Ref<AudioDevice> result = CreateRef<AudioDevice>(phold{ 0 });

		result->Device = alcOpenDevice(DeviceName);
		if (result->Device)
		{
			result->Context = alcCreateContext(result->Device, nullptr);
			if (result->Context)
			{
				if (alcMakeContextCurrent(result->Context))
				{
					const ALCchar* name = nullptr;
					if (alcIsExtensionPresent(result->Device, "ALC_ENUMERATE_ALL_EXT"))
						name = alcGetString(result->Device, ALC_ALL_DEVICES_SPECIFIER);
					if (!name || alcGetError(result->Device) != AL_NO_ERROR)
						name = alcGetString(result->Device, ALC_DEVICE_SPECIFIER);

					DE_CORE_INFO("Using audio source: {0}", name);

					return result;
				}
			}
		}

		return nullptr;
	}

	bool AudioDevice::SetCurrentContext()
	{
		return alcMakeContextCurrent(Context);
	}

	bool AudioDevice::CloseDevice()
	{
		if (!alcMakeContextCurrent(nullptr))
		{
			DE_CORE_ERROR("Failed to set context to nullptr");
			return false;
		}

		alcDestroyContext(Context);

		ALCenum err = alcGetError(Device);
		if (err != AL_NO_ERROR)
		{
			DE_CORE_ERROR("alcDestroyContext() error: {0}", alcGetString(Device, err));
			return false;
		}
		Context = nullptr;

		if (!alcCloseDevice(Device))
		{
			DE_CORE_ERROR("Failed to close sound device");
			return false;
		}

		return true;
	}
}