#include "ContentBrowserTextures.h"

#include "Engine/Asset/AssetManager.h"
#include "../../Asset/Serializer/Serializer.h"

#include <unordered_map>

namespace Engine
{
	static std::unordered_map<std::string, Ref<Texture2D>> LoadedTextures = { };

	Ref<Texture2D> ContentBrowserTextures::LoadTexture(const std::string& AssetID)
	{
		Ref<Texture2D> texture = GetTexture(AssetID);
		if (!texture)
		{
			Asset TextureAsset = AssetManager::LoadAsset(AssetID);

			if (TextureAsset.GetAssetType() == AssetType::Texture)
			{
				texture = Serializer::DeserializeTexture(*TextureAsset.GetData());

				AssetManager::CloseAsset(TextureAsset);

				if (texture)
				{
					LoadedTextures[AssetID] = texture;
					return texture;
				}
			}
		}

		return nullptr;
	}

	Ref<Texture2D> ContentBrowserTextures::GetTexture(const std::string& TextureID)
	{
		auto it = LoadedTextures.find(TextureID);
		if (it != LoadedTextures.end())
			return it->second;

		return nullptr;
	}

	void ContentBrowserTextures::Clear()
	{
		LoadedTextures.clear();
	}
}