#include "depch.h"
#include "VulkanRendererAPI.h"

#include "Engine/Debug/Instrumentor.h"
#include "Engine/Debug/Assert.h"

#include "VulkanApplication.h"

namespace Engine
{
	VulkanRendererAPI::VulkanRendererAPI() : RendererAPI(API::Vulkan)
	{
		if (!VulkanApp)
		{
			DE_LOG(VulkanRendererAPI, "Initializing VulkanRendererAPI...");

			VulkanApp = VulkanApplication::Create();
			PhysicalDevice = VulkanApp->GetMainPhysicalDevice();
			DE_LOG(VulkanRendererAPI, "Vulkan Renderer: {0}", PhysicalDevice.GetDeviceName());

			VulkanPhysicalDevice::QueueFamilyIndices indices = PhysicalDevice.FindQueueFamilies();
			LogicalDevice = PhysicalDevice.CreateLogicalDevice({
				{ indices.GraphicsFamily.value(), 1.f }
			});

			WindowSurface = VulkanApp->CreateWindowSurface();
			DE_ASSERT(WindowSurface, "Failed to create a WindowSurface");

			DE_LOG(VulkanRendererAPI, "Successfully initialized VulkanRendererAPI");
		}
	}
	
	VulkanRendererAPI::~VulkanRendererAPI()
	{
		if (VulkanApp)
		{
			LogicalDevice = nullptr;
			VulkanApp->DestroyWindowSurface(WindowSurface);

			VulkanApp = nullptr;
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
}