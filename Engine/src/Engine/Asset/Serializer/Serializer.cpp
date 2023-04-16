#include "depch.h"
#include "Serializer.h"

#include "Engine/Asset/AssetMetadata.h"

namespace Engine
{
	namespace Serializer
	{
		Ref<AudioBufferData> DeserializeAudio(Ref<AssetMetadata> Metadata)
		{
			MemoryMap& MetadataMap = Metadata->GetFields();

			int Channels = MetadataMap.GetField<int>("Channels");
			int SampleSize = MetadataMap.GetField<int>("SampleSize");
			uint32_t SampleRate = MetadataMap.GetField<uint32_t>("SampleRate");

			Ref<AudioBufferData> result = AudioBufferData::Create(Channels, SampleSize, SampleRate);

			MemoryMap::FieldData FData = MetadataMap.GetFieldData("Data");

			result->SetData(FData.DataPtr, FData.DataSize);

			return result;
		}
	}
}