#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Asset/AssetUtils.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"

#include <istream>

namespace Engine
{
	namespace Serializer
	{
		bool SerializeWithAssetType(AssetMetadata& metadata, std::istream& stream, AssetType type);

		bool SerializeTexture(AssetMetadata& metadata, std::istream& stream);

		Ref<Texture2D> DeserializeTexture(const AssetMetadata& metadata);
	}
}