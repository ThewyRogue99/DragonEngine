#include "depch.h"
#include "TextureManager.h"

#include "Engine/Asset/AssetManager.h"

#include <unordered_map>

namespace Engine
{
	static std::unordered_map<std::string, WeakRef<Texture2D>> LoadedTextureMap = { };

	Ref<Texture2D> DeserializeTexture(Ref<AssetMetadata> Metadata)
	{
		MemoryMap& MetadataMap = Metadata->GetFields();
		int width, height, channels;

		width = MetadataMap.GetField<int>("width");
		height = MetadataMap.GetField<int>("height");
		channels = MetadataMap.GetField<int>("channels");

		MemoryMap::FieldData& data = MetadataMap.GetFieldData("data");

		if (data.DataPtr)
		{
			Ref<Texture2D> result = Texture2D::Create(width, height);
			result->SetData(data.DataPtr, (uint32_t)data.DataSize, (uint32_t)channels);

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

				DE_LOG(
					TextureManager, "Loading texture: {0} with id: {0}",
					TypeUtils::FromUTF16(TextureAsset->GetInfo().Name).c_str(),
					TextureID.c_str()
				);

				if (TextureAsset && TextureAsset->GetInfo().Type == AssetType::Texture)
				{
					Ref<Texture2D> texture = DeserializeTexture(TextureAsset->Metadata);

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

			if (TextureAsset && TextureAsset->GetInfo().Type == AssetType::Texture)
			{
				Ref<Texture2D> texture = DeserializeTexture(TextureAsset->Metadata);

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
		if (TextureAsset && (TextureAsset->GetInfo().Type == AssetType::Texture))
		{
			auto& it = LoadedTextureMap.find(TextureAsset->GetInfo().ID);

			if (it != LoadedTextureMap.end())
			{
				// Check if the texture is already loaded
				if (it->second.expired())
				{
					// Texture is not loaded
					Ref<Texture2D> texture = DeserializeTexture(TextureAsset->Metadata);

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
				Ref<Texture2D> texture = DeserializeTexture(TextureAsset->Metadata);

				if (texture)
				{
					LoadedTextureMap[TextureAsset->GetInfo().ID] = texture;
					return texture;
				}
			}
		}

		return nullptr;
	}
}