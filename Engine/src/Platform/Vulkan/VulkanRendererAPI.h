#pragma once

#include "Engine/Renderer/RendererAPI.h"
#include "VulkanDevice.h"

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Engine
{
	class VulkanApplication;
	class VulkanWindowSurface;
	class VulkanSwapChain;

	class VulkanRendererAPI : public RendererAPI
	{
	public:
		ENGINE_API VulkanRendererAPI();
		ENGINE_API ~VulkanRendererAPI();

		ENGINE_API virtual void Init() override;

		ENGINE_API virtual void SetClearColor(const glm::vec4& color) override;
		ENGINE_API virtual void Clear(Ref<Framebuffer> FramebufferRef) override;

		ENGINE_API virtual void DrawIndexed(const IndexedDrawProperties& Props) override;
		ENGINE_API virtual void DrawLine(const LineDrawProperties& Props) override;

		ENGINE_API virtual void SetLineWidth(float Width) override;

		ENGINE_API virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		Ref<VulkanApplication> GetVulkanApplication() const { return VulkanApp; }
		Ref<VulkanWindowSurface> GetWindowSurface() const { return WindowSurface; }

	private:
		ENGINE_API void PickPhysicalDevice(const std::vector<VulkanPhysicalDevice>& SuitableDevices);

	private:
		Ref<VulkanApplication> VulkanApp;

		VulkanPhysicalDevice PhysicalDevice;
		Ref<VulkanLogicalDevice> LogicalDevice;

		Ref<VulkanWindowSurface> WindowSurface = nullptr;

		Ref<VulkanSwapChain> SwapChain = nullptr;
	};
}