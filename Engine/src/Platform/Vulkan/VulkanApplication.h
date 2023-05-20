#pragma once
#include "Engine/Core/Core.h"

typedef struct VkInstance_T* VkInstance;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Engine
{
	class VulkanPhysicalDevice;

	class VulkanApplication
	{
	private:
		struct phold;

	public:
		explicit VulkanApplication(const phold&)
			: VulkanApplication() { }

		ENGINE_API ~VulkanApplication();

		// Returns the best GPU for rendering
		ENGINE_API VulkanPhysicalDevice GetMainPhysicalDevice() const;
		ENGINE_API VkSurfaceKHR CreateWindowSurface() const;

		ENGINE_API void DestroyWindowSurface(VkSurfaceKHR Surface) const;

		ENGINE_API static Ref<VulkanApplication> Create();

	private:
		ENGINE_API VulkanApplication();

		struct phold {
			explicit phold(int) {}
		};

	private:
		VkInstance AppInstance = nullptr;
	};
}