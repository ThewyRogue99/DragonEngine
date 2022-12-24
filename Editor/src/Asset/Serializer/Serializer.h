#pragma once

#include "Engine/Core/Core.h"

#include "Engine/Audio/AudioBuffer.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Asset/AssetUtils.h"
#include "Engine/Asset/Asset.h"
#include "Engine/Asset/AssetMetadata.h"

#include <istream>

namespace Engine
{
	namespace Serializer
	{
		bool SerializeWithAssetType(const CString& FilePath, AssetMetadata& metadata, AssetType type);

		bool SerializeTexture(const CString& FilePath, AssetMetadata& metadata);

		Ref<Texture2D> DeserializeTexture(const AssetMetadata& metadata);

		bool SerializeAudio(const CString& FilePath, AssetMetadata& metadata);

		AudioBufferData DeserializeAudio(const AssetMetadata& metadata);

		bool CreateAssetFromFile(const CString& TargetPath, const CString& DestinationPath, bool Overwrite = false);
	}
}