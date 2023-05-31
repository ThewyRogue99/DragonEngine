#pragma once

#include "Engine/Core/Core.h"

#include <glm/vec2.hpp>

typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkDevice_T* VkDevice;

namespace Engine
{
	class VulkanLogicalDevice;
	class VulkanWindowSurface;

	typedef struct VSyncProperties
	{
		bool Enabled = false;
		bool UseTripleBuffering = false;
	} VSyncProperties;

	typedef struct SwapChainCreateInfo
	{
		Ref<VulkanLogicalDevice> LogicalDeviceRef = nullptr;
		Ref<VulkanWindowSurface> WindowSurface = nullptr;
		glm::vec2 Extent = glm::vec2(0.f);
		VSyncProperties VSyncProps;
	} SwapChainCreateInfo;

	class VulkanSwapChain
	{
	private:
		struct phold;

	public:
		explicit VulkanSwapChain(const phold&, Ref<VulkanLogicalDevice> DeviceRef, VkSwapchainKHR Instance)
			: VulkanSwapChain(DeviceRef, Instance) { }

		ENGINE_API ~VulkanSwapChain();

		ENGINE_API static Ref<VulkanSwapChain> Create(const SwapChainCreateInfo& Info);

	private:
		VulkanSwapChain(Ref<VulkanLogicalDevice> DeviceRef, VkSwapchainKHR Instance)
			: LogicalDeviceRef(DeviceRef), SwapInstance(Instance) { }

		struct phold {
			explicit phold(int) {}
		};

	private:
		Ref<VulkanLogicalDevice> LogicalDeviceRef = nullptr;
		VkSwapchainKHR SwapInstance = nullptr;
	};
}