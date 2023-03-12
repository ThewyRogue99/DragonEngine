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
		bool SerializeWithAssetType(const CString& FilePath, Ref<AssetMetadata> Metadata, AssetType type);

		bool SerializeTexture(const CString& FilePath, Ref<AssetMetadata> Metadata);

		bool SerializeAudio(const CString& FilePath, Ref<AssetMetadata> Metadata);

		bool CreateAssetFromFile(const CString& TargetPath, const CString& DestinationPath);
	}
}