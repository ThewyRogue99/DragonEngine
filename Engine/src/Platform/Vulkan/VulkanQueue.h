#pragma once

#include "Engine/Core/Core.h"

typedef struct VkQueue_T* VkQueue;

namespace Engine
{
	enum VulkanQueueFamilySupport : uint32_t
	{
		Graphics = BIT(0),
		Presentation = BIT(1)
	};

	typedef struct VulkanQueueFamily
	{
		uint32_t SupportFlags = 0;

		uint32_t FamilyIndex = 0;
	} VulkanQueueFamily;

	class VulkanQueue
	{
	private:
		struct phold;

	public:
		explicit VulkanQueue(const phold&, VkQueue Instance)
			: VulkanQueue(Instance) { }

		friend class VulkanLogicalDevice;

	private:
		VulkanQueue(VkQueue Instance) : QueueInstance(Instance) { }

		struct phold {
			explicit phold(int) {}
		};

	private:
		VkQueue QueueInstance = nullptr;
		VulkanQueueFamily QueueFamily;
	};
}