#pragma once

#include "Engine/Core/Core.h"

#include "Engine/Audio/AudioBuffer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Asset/Asset.h"

#include <istream>

namespace Engine
{
	namespace Serializer
	{
		bool SerializeWithAssetType(const CString& FilePath, AssetMetadata& metadata, AssetType type);

		bool SerializeTexture(const CString& FilePath, AssetMetadata& metadata);

		bool SerializeAudio(const CString& FilePath, AssetMetadata& metadata);

		bool CreateAssetFromFile(const CString& TargetPath, const CString& DestinationPath, bool Overwrite = false);
	}
}