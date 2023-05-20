#pragma once

typedef struct VkQueue_T* VkQueue;

namespace Engine
{
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
	};
}