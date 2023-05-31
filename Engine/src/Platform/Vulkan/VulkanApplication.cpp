#include "depch.h"
#include "VulkanApplication.h"

#include "Engine/Debug/Assert.h"

#include "VulkanDevice.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <algorithm>

namespace Engine
{
	VulkanApplication::VulkanApplication()
	{
		DE_LOG(VulkanApplication, "Creating a VulkanApplication");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

		// TODO: Get Application Name
		appInfo.pApplicationName = "Dragon Engine APP";

		// TODO: Change deprecated VK_MAKE_VERSION to VK_MAKE_API_VERSION
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Dragon Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;

		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

#ifdef ENGINE_PLATFORM_MACOS
	#define REQ_EXT_COUNT glfwExtensionCount + 1
#else
	#define REQ_EXT_COUNT glfwExtensionCount
#endif

		std::vector<const char*> requiredExtensions(REQ_EXT_COUNT);

		// Get supported extensions
		{
			DE_LOG(VulkanApplication, "Checking if required extensions are supported...");

			uint32_t supportedExtensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

			std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

			for (uint32_t i = 0; i < glfwExtensionCount; i++)
			{
				const char* ext = glfwExtensions[i];

				auto it = std::find_if(supportedExtensions.begin(), supportedExtensions.end(), [ext](const VkExtensionProperties& prop)
				{
					return strcmp(prop.extensionName, ext) == 0;
				});

				if (it != supportedExtensions.end())
				{
					requiredExtensions[i] = ext;

					DE_LOG(VulkanApplication, "Using supported extension for Vulkan: {0}", ext);
				}
				else
				{
					DE_ASSERT(false, "Required extension '{0}' for Vulkan is not supported", ext);
				}
			}

#ifdef ENGINE_PLATFORM_MACOS
			requiredExtensions[glfwExtensionCount] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
			createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
		}

		createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		createInfo.enabledLayerCount = 0;

		VkResult result = vkCreateInstance(&createInfo, nullptr, &AppInstance);
		DE_ASSERT(result == VK_SUCCESS, "Failed to create a vulkan instance");

		DE_INFO(VulkanApplication, "Successfully created a VulkanApplication");
	}

	VulkanApplication::~VulkanApplication()
	{
		vkDestroyInstance(AppInstance, nullptr);
	}

	std::vector<VulkanPhysicalDevice> VulkanApplication::GetAvailableDevices() const
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(AppInstance, &deviceCount, nullptr);

		std::vector<VkPhysicalDevice> DeviceHandles(deviceCount);

		vkEnumeratePhysicalDevices(AppInstance, &deviceCount, DeviceHandles.data());

		std::vector<VulkanPhysicalDevice> Devices;

		for (auto& Handle : DeviceHandles)
		{
			VulkanPhysicalDevice Device(Handle);

			Devices.push_back(Device);
		}

		if (Devices.size() <= 0)
			DE_WARN(VulkanApplication, "No GPU with Vulkan support found");

		return Devices;
	}

	Ref<VulkanApplication> VulkanApplication::Create()
	{
		return CreateRef<VulkanApplication>(phold{ 0 });
	}

	std::vector<VulkanPhysicalDevice> VulkanApplication::GetSuitableDevices() const
	{
		std::vector<VulkanPhysicalDevice> AvailableDevices = GetAvailableDevices();

		std::vector<VulkanPhysicalDevice> SuitableDevices;

		for (auto& Device : AvailableDevices)
		{
			if (Device.IsDeviceSuitable())
				SuitableDevices.emplace_back(Device);
		}

		return SuitableDevices;
	}
}