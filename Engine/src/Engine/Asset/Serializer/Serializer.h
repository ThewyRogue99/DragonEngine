#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Audio/AudioBuffer.h"

namespace Engine
{
	class AssetMetadata;

	namespace Serializer
	{
		ENGINE_API Ref<AudioBufferData> DeserializeAudio(Ref<AssetMetadata> Metadata);
	}
}