#include "depch.h"
#include "VulkanWindowSurface.h"

#include "VulkanApplication.h"
#include "VulkanRendererAPI.h"

#include "Engine/Debug/Assert.h"
#include "Engine/Core/Application.h"
#include "Engine/Renderer/Renderer.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine
{
	VulkanWindowSurface::~VulkanWindowSurface()
	{
		vkDestroySurfaceKHR(VulkanAppRef->GetInstance(), SurfaceInstance, nullptr);
	}

	Ref<VulkanWindowSurface> VulkanWindowSurface::Create()
	{
		Application& app = Application::Get();
		Ref<Window> appWindow = app.GetWindow();
		DE_ASSERT(appWindow, "No window exists on Application");

		Ref<VulkanRendererAPI> API = std::dynamic_pointer_cast<VulkanRendererAPI>(Renderer::GetRendererAPI());
		DE_ASSERT(API, "Invalid VulkanRendererAPI");

		Ref<VulkanApplication> ApplicationRef = API->GetVulkanApplication();
		DE_ASSERT(ApplicationRef, "Invalid VulkanApplication");

		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(ApplicationRef->GetInstance(), (GLFWwindow*)(appWindow->GetNativeWindow()), nullptr, &surface) != VK_SUCCESS) {
			return nullptr;
		}

		return CreateRef<VulkanWindowSurface>(VulkanWindowSurface::phold{ 0 }, ApplicationRef, surface);
	}
}