#include "depch.h"
#include "VulkanDevice.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Renderer/Renderer.h"

#include "VulkanRendererAPI.h"
#include "VulkanSwapChain.h"
#include "VulkanWindowSurface.h"

#include <cstdint>
#include <limits>
#include <algorithm>

#include <vulkan/vulkan.h>
#include <vector>
#include <set>

namespace Engine
{
	std::vector<VulkanQueueFamily> VulkanPhysicalDevice::GetQueueFamilies() const
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(DeviceInstance, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(DeviceInstance, &queueFamilyCount, queueFamilies.data());

		std::vector<VulkanQueueFamily> result(queueFamilyCount);

		Ref<VulkanRendererAPI> API = std::dynamic_pointer_cast<VulkanRendererAPI>(Renderer::GetRendererAPI());
		DE_ASSERT(API, "Invaild VulkanRendererAPI");

		Ref<VulkanWindowSurface> WindowSurface = API->GetWindowSurface();
		DE_ASSERT(WindowSurface, "Invaild VulkanWindowSurface");

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			VulkanQueueFamily queueFamily;

			const auto& queueFamilyProps = queueFamilies[i];

			if (queueFamilyProps.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				queueFamily.SupportFlags |= VulkanQueueFamilySupport::Graphics;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(DeviceInstance, i, WindowSurface->GetInstance(), &presentSupport);

			if (presentSupport) {
				queueFamily.SupportFlags |= VulkanQueueFamilySupport::Presentation;
			}

			queueFamily.FamilyIndex = i;
			result[i] = queueFamily;
		}

		return result;
	}

	bool VulkanPhysicalDevice::IsDeviceSuitable() const
	{
		VkPhysicalDeviceFeatures deviceFeatures = GetDeviceFeatures();

		std::vector<VulkanQueueFamily> QueueFamilies = GetQueueFamilies();

		uint32_t requiredFlags = (VulkanQueueFamilySupport::Graphics | VulkanQueueFamilySupport::Presentation);

		bool queueExists = false;
		for (auto& queueFamily : QueueFamilies)
		{
			if ((queueFamily.SupportFlags & requiredFlags) == requiredFlags) {
				queueExists = true;
			}
		}

		return deviceFeatures.geometryShader && queueExists && CheckDeviceExtensionSupport();
	}

	typedef struct PhysicalDeviceSurfaceProperties
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	} PhysicalDeviceSurfaceProperties;

	PhysicalDeviceSurfaceProperties VulkanPhysicalDevice::GetPhysicalDeviceSurfaceProperties(Ref<VulkanWindowSurface> WindowSurface) const
	{
		PhysicalDeviceSurfaceProperties props;

		VkSurfaceKHR SurfaceInstance = WindowSurface->GetInstance();

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(DeviceInstance, SurfaceInstance, &props.Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(DeviceInstance, SurfaceInstance, &formatCount, nullptr);

		if (formatCount != 0)
		{
			props.Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(DeviceInstance, SurfaceInstance, &formatCount, props.Formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(DeviceInstance, SurfaceInstance, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			props.PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(DeviceInstance, SurfaceInstance, &presentModeCount, props.PresentModes.data());
		}

		return props;
	}

	bool VulkanPhysicalDevice::IsWindowSurfaceSupported(Ref<VulkanWindowSurface> WindowSurface) const
	{
		PhysicalDeviceSurfaceProperties props = GetPhysicalDeviceSurfaceProperties(WindowSurface);

		return !props.Formats.empty() && !props.PresentModes.empty();
	}

	VkPhysicalDeviceProperties VulkanPhysicalDevice::GetDeviceProperties() const
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(DeviceInstance, &deviceProperties);

		return deviceProperties;
	}

	VkPhysicalDeviceFeatures VulkanPhysicalDevice::GetDeviceFeatures() const
	{
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(DeviceInstance, &deviceFeatures);

		return deviceFeatures;
	}

	Ref<VulkanLogicalDevice> VulkanPhysicalDevice::CreateLogicalDevice(std::initializer_list<QueueCreateInfo> QueueCreateInfos)
	{
		std::vector<VkDeviceQueueCreateInfo> _QueueCreateInfos(QueueCreateInfos.size());

		// Set vector
		{
			int i = 0;
			for (auto& createInfo : QueueCreateInfos)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = createInfo.QueueFamily.FamilyIndex;
				queueCreateInfo.queueCount = 1;

				queueCreateInfo.pQueuePriorities = &createInfo.Priority;

				_QueueCreateInfos[i] = queueCreateInfo;

				i++;
			}
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.pQueueCreateInfos = _QueueCreateInfos.data();
		createInfo.queueCreateInfoCount = (uint32_t)(_QueueCreateInfos.size());

		createInfo.pEnabledFeatures = &deviceFeatures;

		const std::vector<const char*> RequiredDeviceExtensions = GetRequiredDeviceExtensions();

		createInfo.enabledExtensionCount = (uint32_t)(RequiredDeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = RequiredDeviceExtensions.data();

		createInfo.enabledLayerCount = 0;

		VkDevice device;
		DE_ASSERT(vkCreateDevice(DeviceInstance, &createInfo, nullptr, &device) == VK_SUCCESS, "Failed to create a logical device");

		return CreateRef<VulkanLogicalDevice>(
			VulkanLogicalDevice::phold{ 0 },
			VulkanPhysicalDevice(DeviceInstance),
			device
		);
	}

	CString VulkanPhysicalDevice::GetDeviceName() const
	{
		VkPhysicalDeviceProperties deviceProperties = GetDeviceProperties();

		return deviceProperties.deviceName;
	}

	const std::vector<const char*> VulkanPhysicalDevice::GetRequiredDeviceExtensions() const
	{
		// Set this vector with the extensions that are required for the device
		return {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	}

	bool VulkanPhysicalDevice::CheckDeviceExtensionSupport() const
	{
		const std::vector<const char*> deviceExtensions = GetRequiredDeviceExtensions();

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(DeviceInstance, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(DeviceInstance, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	VulkanLogicalDevice::~VulkanLogicalDevice()
	{
		if (DeviceInstance)
		{
			vkDestroyDevice(DeviceInstance, nullptr);
		}
	}

	VulkanQueue VulkanLogicalDevice::GetQueue(uint32_t QueueFamilyIndex)
	{
		VkQueue hQueue;
		vkGetDeviceQueue(DeviceInstance, QueueFamilyIndex, 0, &hQueue);

		return VulkanQueue(VulkanQueue::phold{ 0 }, hQueue);
	}
}