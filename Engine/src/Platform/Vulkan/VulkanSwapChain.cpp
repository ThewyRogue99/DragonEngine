#include "depch.h"
#include "VulkanSwapChain.h"

#include "Engine/Debug/Assert.h"

#include "VulkanDevice.h"
#include "VulkanWindowSurface.h"

#include <vulkan/vulkan.h>

namespace Engine
{
	typedef struct PhysicalDeviceSurfaceProperties
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	} PhysicalDeviceSurfaceProperties;

	VulkanSwapChain::~VulkanSwapChain()
	{
		vkDestroySwapchainKHR(LogicalDeviceRef->GetInstance(), SwapInstance, nullptr);
	}

	Ref<VulkanSwapChain> VulkanSwapChain::Create(const SwapChainCreateInfo& Info)
	{
		DE_ASSERT(
			Info.LogicalDeviceRef &&
			Info.WindowSurface &&
			Info.Extent != glm::vec2(0.f),
			"Invalid SwapChainCreateInfo"
		);

		Ref<VulkanLogicalDevice> DeviceRef = Info.LogicalDeviceRef;

		PhysicalDeviceSurfaceProperties props = DeviceRef->GetPhysicalDevice().GetPhysicalDeviceSurfaceProperties(Info.WindowSurface);

		DE_ASSERT(props.Formats.size() > 0, "No available surface format found for this device");
		DE_ASSERT(props.PresentModes.size() > 0, "No available present mode found for this device");
		std::optional<VkSurfaceFormatKHR> SwapSurfaceFormat;

		// Set SwapSurfaceFormat
		{
			for (const auto& sFormat : props.Formats)
			{
				if (sFormat.format == VK_FORMAT_B8G8R8A8_SRGB && sFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					SwapSurfaceFormat = sFormat;
				}
			}

			if (!SwapSurfaceFormat.has_value())
			{
				SwapSurfaceFormat = props.Formats[0];
			}
		}

		VkPresentModeKHR SwapPresentMode;

		// Set SwapPresentMode
		{
			if (Info.VSyncProps.Enabled)
			{
				SwapPresentMode = VK_PRESENT_MODE_FIFO_KHR;

				if (Info.VSyncProps.UseTripleBuffering)
				{
					for (const auto& presentMode : props.PresentModes)
					{
						if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
						{
							SwapPresentMode = presentMode;
						}
					}
				}

				if (SwapPresentMode == VK_PRESENT_MODE_FIFO_KHR)
					DE_WARN(VulkanDevice, "Requested tripple buffered swap chain on a device that does not support it.");
			}
			else
			{
				SwapPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			}
		}

		VkExtent2D SwapExtent;

		// Set SwapExtent
		{
			if (props.Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				SwapExtent = props.Capabilities.currentExtent;
			}
			else
			{
				SwapExtent = {
					(uint32_t)(Info.Extent.x),
					(uint32_t)(Info.Extent.y)
				};

				SwapExtent.width = std::clamp(SwapExtent.width, props.Capabilities.minImageExtent.width, props.Capabilities.maxImageExtent.width);
				SwapExtent.height = std::clamp(SwapExtent.height, props.Capabilities.minImageExtent.height, props.Capabilities.maxImageExtent.height);
			}
		}

		uint32_t imageCount = props.Capabilities.minImageCount + 1;

		if (props.Capabilities.maxImageCount > 0 && imageCount > props.Capabilities.maxImageCount) {
			imageCount = props.Capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = Info.WindowSurface->GetInstance();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = SwapSurfaceFormat.value().format;
		createInfo.imageColorSpace = SwapSurfaceFormat.value().colorSpace;
		createInfo.imageExtent = SwapExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional

		createInfo.preTransform = props.Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = SwapPresentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkSwapchainKHR swapChain;
		DE_ASSERT(vkCreateSwapchainKHR(DeviceRef->GetInstance(), &createInfo, nullptr, &swapChain) == VK_SUCCESS, "Failed to create a VulkanSwapChain");

		return CreateRef<VulkanSwapChain>(VulkanSwapChain::phold{ 0 }, DeviceRef, swapChain);
	}
}