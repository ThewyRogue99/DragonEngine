#pragma once
#include "Engine/Core/Core.h"

typedef struct VkInstance_T* VkInstance;

#include <vector>

namespace Engine
{
	class VulkanPhysicalDevice;

	class VulkanApplication
	{
	private:
		struct phold;

	public:
		explicit VulkanApplication(const phold&)
			: VulkanApplication() { }

		ENGINE_API ~VulkanApplication();

		inline VkInstance GetInstance() const { return AppInstance; }
		ENGINE_API std::vector<VulkanPhysicalDevice> GetAvailableDevices() const;
		ENGINE_API std::vector<VulkanPhysicalDevice> GetSuitableDevices() const;

		ENGINE_API static Ref<VulkanApplication> Create();

	private:
		ENGINE_API VulkanApplication();

		struct phold {
			explicit phold(int) {}
		};

	private:
		VkInstance AppInstance = nullptr;
	};
}