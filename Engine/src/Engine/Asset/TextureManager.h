#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Renderer/Texture.h"
#include "Engine/Asset/Asset.h"

namespace Engine
{
	class TextureManager
	{
	public:
		static Ref<Texture2D> LoadTexture(const CString& Path, const CString& Name);
		static Ref<Texture2D> LoadTexture(const std::string& TextureID);
		static Ref<Texture2D> LoadTexture(Ref<Asset> TextureAsset);
	};
}