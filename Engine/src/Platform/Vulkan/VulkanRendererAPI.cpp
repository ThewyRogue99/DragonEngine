#include "depch.h"
#include "VulkanRendererAPI.h"

#include "Engine/Core/Application.h"
#include "Engine/Debug/Instrumentor.h"
#include "Engine/Debug/Assert.h"

#include "VulkanApplication.h"
#include "VulkanWindowSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanQueue.h"

#include <vulkan/vulkan.h>
#include <optional>

namespace Engine
{
	VulkanRendererAPI::VulkanRendererAPI() : RendererAPI(API::Vulkan)
	{
		
	}
	
	VulkanRendererAPI::~VulkanRendererAPI()
	{
		if (VulkanApp)
		{
			LogicalDevice = nullptr;
			WindowSurface = nullptr;

			VulkanApp = nullptr;
		}
	}

	void VulkanRendererAPI::Init()
	{
		if (!VulkanApp)
		{
			DE_LOG(VulkanRendererAPI, "Initializing VulkanRendererAPI...");

			VulkanApp = VulkanApplication::Create();

			WindowSurface = VulkanWindowSurface::Create();
			DE_ASSERT(WindowSurface, "Failed to create a WindowSurface");

			std::vector<VulkanPhysicalDevice> SuitableDevices = VulkanApp->GetSuitableDevices();
			PickPhysicalDevice(SuitableDevices);

			std::vector<VulkanQueueFamily> QueueFamilies = PhysicalDevice.GetQueueFamilies();

			uint32_t requiredFlags = (VulkanQueueFamilySupport::Graphics | VulkanQueueFamilySupport::Presentation);

			VulkanQueueFamily* selectedQueue = nullptr;
			for (auto& queueFamily : QueueFamilies)
			{
				if ((queueFamily.SupportFlags & requiredFlags) == requiredFlags) {
					selectedQueue = &queueFamily;
				}
			}

			LogicalDevice = PhysicalDevice.CreateLogicalDevice({
				{ *selectedQueue, 1.f }
			});

			Application& app = Application::Get();
			Ref<Window> AppWindow = app.GetWindow();

			SwapChainCreateInfo SwapInfo;
			SwapInfo.LogicalDeviceRef = LogicalDevice;
			SwapInfo.WindowSurface = WindowSurface;
			SwapInfo.Extent = AppWindow->GetFrameSize();

			// TODO: Change this in the RendererAPI
			VSyncProperties VProps;
			VProps.Enabled = true;
			VProps.UseTripleBuffering = true;

			SwapInfo.VSyncProps = VProps;

			SwapChain = VulkanSwapChain::Create(SwapInfo);

			DE_LOG(VulkanRendererAPI, "Successfully initialized VulkanRendererAPI");
		}
	}
	
	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		
	}
	
	void VulkanRendererAPI::Clear(Ref<Framebuffer> FramebufferRef)
	{
		
	}
	
	void VulkanRendererAPI::DrawIndexed(const IndexedDrawProperties& Props)
	{
		
	}
	
	void VulkanRendererAPI::DrawLine(const LineDrawProperties& Props)
	{
		
	}
	
	void VulkanRendererAPI::SetLineWidth(float Width)
	{
		
	}
	
	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		
	}
	
	void VulkanRendererAPI::PickPhysicalDevice(const std::vector<VulkanPhysicalDevice>& SuitableDevices)
	{
		DE_ASSERT(WindowSurface, "A vaild Vulkan Window Surface must be created before picking a device");

		std::optional<VulkanPhysicalDevice> integratedDevice;

		// Pick a discrete GPU
		for (auto& Device : SuitableDevices)
		{
			VkPhysicalDeviceProperties deviceProperties = Device.GetDeviceProperties();

			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && !integratedDevice.has_value())
			{
				integratedDevice = Device;
			}

			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				PhysicalDevice = Device;
				break;
			}
		}

		// Pick an integrated GPU if no discrete GPU is found
		if (!PhysicalDevice.IsVaild() && integratedDevice.has_value()) {
			PhysicalDevice = integratedDevice.value();
		}

		DE_ASSERT(PhysicalDevice.IsVaild(), "No suitable GPU found");
		DE_LOG(VulkanRendererAPI, "Vulkan Renderer: {0}", PhysicalDevice.GetDeviceName());
	}
}