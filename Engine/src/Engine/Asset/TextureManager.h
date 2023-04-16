#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class Asset;

	class ENGINE_API TextureManager
	{
	public:
		static Ref<Texture2D> LoadTexture(const WString& Path, const WString& Name);
		static Ref<Texture2D> LoadTexture(const CString& TextureID);
		static Ref<Texture2D> LoadTexture(Ref<Asset> TextureAsset);
	};
}