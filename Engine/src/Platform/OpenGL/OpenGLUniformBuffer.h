#pragma once

#include "Engine/Renderer/UniformBuffer.h"

namespace Engine
{
	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		ENGINE_API OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		ENGINE_API virtual ~OpenGLUniformBuffer();

		ENGINE_API virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		uint32_t BufferID = 0;
	};
}