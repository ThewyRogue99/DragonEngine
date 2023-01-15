#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

struct ALCdevice;
struct ALCcontext;

namespace Engine
{
	class AudioDevice
	{
	private:
		struct phold;

	public:
		explicit AudioDevice(const phold&) : AudioDevice() { };

		~AudioDevice() { CloseDevice(); };

		friend class AudioEngine;

		static Ref<AudioDevice> OpenDevice(const char* DeviceName = nullptr);

		bool SetCurrentContext();

		bool CloseDevice();

		const std::string& GetName() const { return DeviceName; }

		AudioDevice(const AudioDevice&) = delete;
		const AudioDevice& operator =(const AudioDevice&) = delete;

	private:
		AudioDevice() = default;

		struct phold {
			explicit phold(int) {}
		};

	private:
		ALCdevice* Device = nullptr;
		ALCcontext* Context = nullptr;

		std::string DeviceName;
	};
}