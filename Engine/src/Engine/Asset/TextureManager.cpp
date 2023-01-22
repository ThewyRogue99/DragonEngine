#include "depch.h"
#include "TextureManager.h"

#include "Engine/Asset/AssetManager.h"

#include <unordered_map>

namespace Engine
{
	static std::unordered_map<std::string, WeakRef<Texture2D>> LoadedTextureMap = { };

	Ref<Texture2D> DeserializeTexture(const AssetMetadata& metadata)
	{
		int width, height, channels;

		width = metadata.GetField<int>("width");
		height = metadata.GetField<int>("height");
		channels = metadata.GetField<int>("channels");

		size_t dataSize = (size_t)(width * height * channels);

		void* data = metadata.GetField("data", dataSize);

		if (data)
		{
			Ref<Texture2D> result = Texture2D::Create(width, height);
			result->SetData(data, (uint32_t)dataSize, (uint32_t)channels);

			return result;
		}

		return nullptr;
	}

	Ref<Texture2D> TextureManager::LoadTexture(const CString& Path, const CString& Name)
	{
		return LoadTexture(AssetManager::GetAssetID(Path, Name));
	}

	Ref<Texture2D> TextureManager::LoadTexture(const std::string& TextureID)
	{
		auto& it = LoadedTextureMap.find(TextureID);

		if (it != LoadedTextureMap.end())
		{
			// Check if the texture is already loaded
			if (it->second.expired())
			{
				// Texture is not loaded
				Ref<Asset> TextureAsset = AssetManager::LoadAsset(TextureID);

				if (TextureAsset && TextureAsset->GetAssetType() == AssetType::Texture)
				{
					Ref<Texture2D> texture = DeserializeTexture(TextureAsset->GetData());

					if (texture)
					{
						it->second = texture;
						return texture;
					}
				}
			}
			else
			{
				// Texture is loaded
				return it->second.lock();
			}
		}
		else
		{
			Ref<Asset> TextureAsset = AssetManager::LoadAsset(TextureID);

			if (TextureAsset && TextureAsset->GetAssetType() == AssetType::Texture)
			{
				Ref<Texture2D> texture = DeserializeTexture(TextureAsset->GetData());

				if (texture)
				{
					LoadedTextureMap[TextureID] = texture;
					return texture;
				}
			}
		}

		return nullptr;
	}

	Ref<Texture2D> TextureManager::LoadTexture(Ref<Asset> TextureAsset)
	{
		if (TextureAsset && (TextureAsset->GetAssetType() == AssetType::Texture))
		{
			auto& it = LoadedTextureMap.find(TextureAsset->GetID());

			if (it != LoadedTextureMap.end())
			{
				// Check if the texture is already loaded
				if (it->second.expired())
				{
					// Texture is not loaded
					Ref<Texture2D> texture = DeserializeTexture(TextureAsset->GetData());

					if (texture)
					{
						it->second = texture;
						return texture;
					}
				}
				else
				{
					// Texture is loaded
					return it->second.lock();
				}
			}
			else
			{
				Ref<Texture2D> texture = DeserializeTexture(TextureAsset->GetData());

				if (texture)
				{
					LoadedTextureMap[TextureAsset->GetID()] = texture;
					return texture;
				}
			}
		}

		return nullptr;
	}
}