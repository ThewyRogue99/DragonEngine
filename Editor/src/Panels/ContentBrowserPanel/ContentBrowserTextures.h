#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class ContentBrowserTextures
	{
	public:
		static Ref<Texture2D> LoadTexture(const std::string& AssetID);

		static Ref<Texture2D> GetTexture(const std::string& TextureID);

		static void Clear();
	};
}