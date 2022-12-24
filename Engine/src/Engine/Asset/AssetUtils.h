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
		AssetType GetAssetTypeFromExtension(const CString& Extension);
	}
}