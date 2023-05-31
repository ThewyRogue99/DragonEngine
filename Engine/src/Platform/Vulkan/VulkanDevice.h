#pragma once
#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "VulkanQueue.h"

#include <optional>
#include <initializer_list>

typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;

typedef struct VkPhysicalDeviceProperties VkPhysicalDeviceProperties;
typedef struct VkPhysicalDeviceFeatures VkPhysicalDeviceFeatures;
typedef struct VkSurfaceCapabilitiesKHR VkSurfaceCapabilitiesKHR;
typedef struct VkSurfaceFormatKHR VkSurfaceFormatKHR;

typedef enum VkPresentModeKHR VkPresentModeKHR;

namespace Engine
{
	class VulkanQueue;
	class VulkanSwapChain;
	class VulkanLogicalDevice;
	class VulkanWindowSurface;

	typedef struct VulkanQueueFamily VulkanQueueFamily;
	typedef struct PhysicalDeviceSurfaceProperties PhysicalDeviceSurfaceProperties;

	typedef struct QueueCreateInfo
	{
		VulkanQueueFamily QueueFamily;
		float Priority = 1.f;
	} QueueCreateInfo;

	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDevice() = default;
		VulkanPhysicalDevice(const VulkanPhysicalDevice&) = default;

		ENGINE_API std::vector<VulkanQueueFamily> GetQueueFamilies() const;

		ENGINE_API bool IsDeviceSuitable() const;
		ENGINE_API bool IsWindowSurfaceSupported(Ref<VulkanWindowSurface> WindowSurface) const;
		inline bool IsVaild() const { return DeviceInstance; }

		VkPhysicalDevice GetInstance() const { return DeviceInstance; }
		ENGINE_API CString GetDeviceName() const;
		ENGINE_API VkPhysicalDeviceProperties GetDeviceProperties() const;
		ENGINE_API VkPhysicalDeviceFeatures GetDeviceFeatures() const;
		ENGINE_API PhysicalDeviceSurfaceProperties GetPhysicalDeviceSurfaceProperties(Ref<VulkanWindowSurface> WindowSurface) const;

		ENGINE_API Ref<VulkanLogicalDevice> CreateLogicalDevice(std::initializer_list<QueueCreateInfo> QueueCreateInfos);

		friend class VulkanApplication;

	private:
		VulkanPhysicalDevice(VkPhysicalDevice Instance) : DeviceInstance(Instance) { }

		ENGINE_API const std::vector<const char*> GetRequiredDeviceExtensions() const;
		ENGINE_API bool CheckDeviceExtensionSupport() const;

	private:
		VkPhysicalDevice DeviceInstance = nullptr;
	};

	class VulkanLogicalDevice
	{
	private:
		struct phold;

	public:
		explicit VulkanLogicalDevice(const phold&, const VulkanPhysicalDevice& PhysicalDevice, VkDevice Instance)
			: VulkanLogicalDevice(PhysicalDevice, Instance) { }

		ENGINE_API ~VulkanLogicalDevice();

		VkDevice GetInstance() const { return DeviceInstance; }
		VulkanPhysicalDevice GetPhysicalDevice() const { return _PhysicalDevice; }
		ENGINE_API VulkanQueue GetQueue(uint32_t QueueFamilyIndex);

		friend class VulkanPhysicalDevice;

	private:
		VulkanLogicalDevice(const VulkanPhysicalDevice& PhysicalDevice, VkDevice Instance)
			: _PhysicalDevice(PhysicalDevice), DeviceInstance(Instance) { }

		struct phold {
			explicit phold(int) {}
		};

	private:
		VkDevice DeviceInstance = nullptr;
		VulkanPhysicalDevice _PhysicalDevice;
	};
}