#pragma once

#include "Engine/Core/Core.h"

#include "Engine/Audio/AudioBuffer.h"

#include "Engine/Asset/Asset.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	namespace Serializer
	{
		Ref<Texture2D> DeserializeTexture(const AssetMetadata& metadata);

		AudioBufferData DeserializeAudio(const AssetMetadata& metadata);
	}
}