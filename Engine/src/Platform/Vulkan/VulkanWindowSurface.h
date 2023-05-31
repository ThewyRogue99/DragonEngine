#pragma once

#include "Engine/Core/Core.h"

typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace Engine
{
	class VulkanApplication;

	class VulkanWindowSurface
	{
	private:
		struct phold;

	public:
		explicit VulkanWindowSurface(const phold&, Ref<VulkanApplication> ApplicationRef, VkSurfaceKHR Instance)
			: VulkanWindowSurface(ApplicationRef, Instance) { }

		ENGINE_API ~VulkanWindowSurface();

		ENGINE_API static Ref<VulkanWindowSurface> Create();

		inline VkSurfaceKHR GetInstance() const { return SurfaceInstance; }

	private:
		VulkanWindowSurface(Ref<VulkanApplication> ApplicationRef, VkSurfaceKHR Instance)
			: VulkanAppRef(ApplicationRef), SurfaceInstance(Instance) { }

		struct phold {
			explicit phold(int) {}
		};

	private:
		Ref<VulkanApplication> VulkanAppRef = nullptr;
		VkSurfaceKHR SurfaceInstance = nullptr;
	};
}