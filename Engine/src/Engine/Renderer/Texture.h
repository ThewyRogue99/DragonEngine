#pragma once

#include "Engine/Core/Core.h"

namespace Engine
{
	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual uint32_t GetRendererID() const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		ENGINE_API static Ref<Texture2D> Create(uint32_t width, uint32_t height, uint32_t channels = 4);

		virtual bool operator ==(Texture2D& texture) = 0;
	};
}
