#pragma once

#include "Engine/Core/Core.h"

namespace Engine
{
	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() { }
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		ENGINE_API static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};

}