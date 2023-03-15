#include "ResourceTool.h"

#include "Engine/Asset/AssetUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <unordered_map>
#include <filesystem>

#define ICON_PATH L"Resource/Icon"

namespace Engine
{
	std::unordered_map<CString, Ref<Texture2D>> IconMap = { };

	void ResourceTool::LoadResources()
	{
		LoadIcons();
	}

	static void LoadDirIcons(std::filesystem::path Path = L"")
	{
		for (auto& entry : std::filesystem::directory_iterator(Path))
		{
			std::filesystem::path entryPath = entry.path();
			if (entry.is_directory())
				LoadDirIcons(entryPath);

			std::filesystem::path ext = entryPath.extension();

			AssetType type = AssetUtils::GetAssetTypeFromExtension(ext);
			if (type == AssetType::Texture)
			{
				stbi_set_flip_vertically_on_load(true);

				std::string path_str = TypeUtils::FromUTF16(entryPath);

				int width, height, channels;
				unsigned char* data = stbi_load(path_str.c_str(), &width, &height, &channels, 0);
				if (data)
				{
					Ref<Texture2D> IconTexture = Texture2D::Create(width, height, channels);
					IconTexture->SetData(data, width * height * channels);

					IconMap[entryPath.stem()] = IconTexture;
				}

				stbi_image_free(data);
			}
		}
	}

	void ResourceTool::LoadIcons()
	{
		LoadDirIcons(ICON_PATH);
	}

	Ref<Texture2D> ResourceTool::GetIcon(const CString& IconName)
	{
		return IconMap[IconName];
	}
}