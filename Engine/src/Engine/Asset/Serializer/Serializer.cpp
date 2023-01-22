#include "depch.h"
#include "Serializer.h"

namespace Engine
{
	namespace Serializer
	{
		AudioBufferData DeserializeAudio(const AssetMetadata& metadata)
		{
			AudioBufferData result;

			result.Channels = metadata.GetField<int>("Channels");
			result.SampleSize = metadata.GetField<int>("SampleSize");
			result.SampleRate = metadata.GetField<uint32_t>("SampleRate");
			result.Data = metadata.GetField("Data", result.DataSize);

			return result;
		}
	}
}