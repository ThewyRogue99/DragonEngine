#include "ResourceTool.h"

#include "Engine/Asset/AssetUtils.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <unordered_map>
#include <filesystem>

#include <imgui.h>

#include "IconsFontAwesome6.h"

#define ICON_PATH L"Resource/Icon"

namespace Engine
{
	std::unordered_map<WString, Ref<Texture2D>> ImageIconMap = { };

	std::unordered_map<FontIconType, ImFont*> FontIconMap = { };

	void ResourceTool::LoadResources()
	{
		LoadImageIcons();
		LoadFontIcons();
	}

	static void LoadDirImageIcons(std::filesystem::path Path = L"")
	{
		for (auto& entry : std::filesystem::directory_iterator(Path))
		{
			std::filesystem::path entryPath = entry.path();
			if (entry.is_directory())
				LoadDirImageIcons(entryPath);

			std::filesystem::path ext = entryPath.extension();

			AssetType type = AssetUtils::GetAssetTypeFromExtension(ext);
			if (type == AssetType::Texture)
			{
				stbi_set_flip_vertically_on_load(true);

				CString path_str = TypeUtils::FromUTF16(entryPath);

				int width, height, channels;
				unsigned char* data = stbi_load(path_str.c_str(), &width, &height, &channels, 0);
				if (data)
				{
					Ref<Texture2D> IconTexture = Texture2D::Create(width, height, channels);
					IconTexture->SetData(data, width * height * channels);

					ImageIconMap[entryPath.stem()] = IconTexture;
				}

				stbi_image_free(data);
			}
		}
	}

	void ResourceTool::LoadFontIcons()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Load FontAwesome6
		{
			static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

			ImFontConfig conf;
			conf.MergeMode = true;

			// Regular
			ImFont* Regular = io.Fonts->AddFontFromFileTTF(
				"Resource/Fonts/FontAwesome6/Font Awesome 6 Pro-Regular-400.otf",
				16.0f,
				&conf,
				icon_ranges
			);

			FontIconMap[FontIconType::FontAwesome6_Regular] = Regular;
		}
	}

	void ResourceTool::LoadImageIcons()
	{
		LoadDirImageIcons(ICON_PATH);
	}

	Ref<Texture2D> ResourceTool::GetImageIcon(const WString& IconName)
	{
		return ImageIconMap[IconName];
	}

	ImFont* ResourceTool::GetFontIcon(FontIconType Type)
	{
		return FontIconMap[Type];
	}
}