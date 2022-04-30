#pragma once

#include "Engine/Core.h"

namespace Engine
{
	class ENGINE_API Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(const char* path);

		static Ref<Texture2D> Create(std::string& path);
	};
}
