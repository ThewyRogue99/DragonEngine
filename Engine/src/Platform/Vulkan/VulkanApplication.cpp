#include "depch.h"
#include "VulkanApplication.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Core/Application.h"

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

	static VkPhysicalDevice PickPhysicalDevice(const std::vector<VkPhysicalDevice>& CompatibleDevices)
	{
		DE_ASSERT(CompatibleDevices.size() > 0, "No compatible GPU found");

		for (auto device : CompatibleDevices)
		{
			// Get device properties
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				return device;
		}

		return CompatibleDevices[0];
	}

	VulkanPhysicalDevice VulkanApplication::GetMainPhysicalDevice() const
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(AppInstance, &deviceCount, nullptr);

		DE_ASSERT(deviceCount > 0, "Failed to find a GPU with Vulkan support");

		if (deviceCount == 1)
		{
			VkPhysicalDevice device;
			vkEnumeratePhysicalDevices(AppInstance, &deviceCount, &device);

			return VulkanPhysicalDevice(device);
		}

		std::vector<VkPhysicalDevice> Devices(deviceCount);
		std::vector<VkPhysicalDevice> CompatibleDevices;

		vkEnumeratePhysicalDevices(AppInstance, &deviceCount, Devices.data());

		// Set compatible devices
		for (uint32_t i = 0; i < deviceCount; i++)
		{
			VkPhysicalDevice device = Devices[i];

			// Get device features
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			VulkanPhysicalDevice DeviceHandle(device);

			if (deviceFeatures.geometryShader && DeviceHandle.FindQueueFamilies().IsComplete())
				CompatibleDevices.push_back(device);
		}

		return VulkanPhysicalDevice(PickPhysicalDevice(CompatibleDevices));
	}

	VkSurfaceKHR VulkanApplication::CreateWindowSurface() const
	{
		Application& app = Application::Get();
		Ref<Window> appWindow = app.GetWindow();

		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(AppInstance, (GLFWwindow*)appWindow->GetNativeWindow(), nullptr, &surface) != VK_SUCCESS) {
			return nullptr;
		}

		return surface;
	}

	void VulkanApplication::DestroyWindowSurface(VkSurfaceKHR Surface) const
	{
		vkDestroySurfaceKHR(AppInstance, Surface, nullptr);
	}

	Ref<VulkanApplication> VulkanApplication::Create()
	{
		return CreateRef<VulkanApplication>(phold{ 0 });
	}
}