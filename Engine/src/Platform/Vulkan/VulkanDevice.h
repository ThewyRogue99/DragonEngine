#pragma once
#include "Engine/Core/Core.h"

#include "VulkanQueue.h"

typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;

struct VkPhysicalDeviceFeatures;

#include <optional>
#include <initializer_list>

namespace Engine
{
	struct QueueCreateInfo
	{
		uint32_t FamilyIndex;
		float Priority = 1.f;
	};

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice(VkPhysicalDevice Instance) : DeviceInstance(Instance) { }

		struct QueueFamilyIndices;

		ENGINE_API QueueFamilyIndices FindQueueFamilies() const;

		ENGINE_API Ref<VulkanLogicalDevice> CreateLogicalDevice(std::initializer_list<QueueCreateInfo> QueueCreateInfos);

		ENGINE_API const char* GetDeviceName() const;

	public:
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> GraphicsFamily;

			bool IsComplete()
			{
				return GraphicsFamily.has_value();
			}
		};

	private:
		VkPhysicalDevice DeviceInstance = nullptr;
	};

	class VulkanLogicalDevice
	{
	private:
		struct phold;

	public:
		explicit VulkanLogicalDevice(const phold&, VkDevice Instance)
			: VulkanLogicalDevice(Instance) { }

		ENGINE_API ~VulkanLogicalDevice();

		ENGINE_API VulkanQueue GetQueue(uint32_t QueueFamilyIndex);

		friend class VulkanPhysicalDevice;

	private:
		VulkanLogicalDevice(VkDevice Instance) : DeviceInstance(Instance) { }

		struct phold {
			explicit phold(int) {}
		};

	private:
		VkDevice DeviceInstance = nullptr;
	};
}