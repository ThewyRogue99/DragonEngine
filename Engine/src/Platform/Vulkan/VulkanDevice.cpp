#include "depch.h"
#include "VulkanDevice.h"

#include "Engine/Debug/Assert.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::FindQueueFamilies() const
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(DeviceInstance, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(DeviceInstance, &queueFamilyCount, queueFamilies.data());

		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			const auto& queueFamily = queueFamilies[i];

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}
		}

		return indices;
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
				queueCreateInfo.queueFamilyIndex = createInfo.FamilyIndex;
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

		createInfo.enabledExtensionCount = 0;
		createInfo.enabledLayerCount = 0;

		VkDevice device;
		DE_ASSERT(vkCreateDevice(DeviceInstance, &createInfo, nullptr, &device) == VK_SUCCESS, "Failed to create a logical device");

		return CreateRef<VulkanLogicalDevice>(VulkanLogicalDevice::phold{ 0 }, device);
	}

	const char* VulkanPhysicalDevice::GetDeviceName() const
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(DeviceInstance, &deviceProperties);

		return deviceProperties.deviceName;
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