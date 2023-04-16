#pragma once

#include "Engine/Types/Types.h"

namespace Engine
{
	enum class AssetType : uint32_t
	{
		Undefined,
		Folder,
		Texture,
		Scene,
		Script,
		Audio
	};

	namespace AssetUtils
	{
		ENGINE_API AssetType GetAssetTypeFromExtension(const WString& Extension);
	}
}