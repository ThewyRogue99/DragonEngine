#include "depch.h"
#include "Serializer.h"

namespace Engine
{
	namespace Serializer
	{
		AudioBufferData DeserializeAudio(Ref<AssetMetadata> Metadata)
		{
			AudioBufferData result;
			MemoryMap& MetadataMap = Metadata->GetFields();

			result.Channels = MetadataMap.GetField<int>("Channels");
			result.SampleSize = MetadataMap.GetField<int>("SampleSize");
			result.SampleRate = MetadataMap.GetField<uint32_t>("SampleRate");

			MemoryMap::FieldData FData = MetadataMap.GetFieldData("Data");
			
			memcpy((void*)result.Data, FData.DataPtr, FData.DataSize);
			result.DataSize = FData.DataSize;

			return result;
		}
	}
}