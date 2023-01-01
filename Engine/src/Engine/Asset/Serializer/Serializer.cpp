#include "depch.h"
#include "Serializer.h"

namespace Engine
{
	namespace Serializer
	{
		Ref<Texture2D> DeserializeTexture(const AssetMetadata& metadata)
		{
			int width, height, channels;

			width = metadata.GetField<int>("width");
			height = metadata.GetField<int>("height");
			channels = metadata.GetField<int>("channels");

			size_t dataSize = (size_t)(width * height * channels);

			void* data = metadata.GetField("data", dataSize);

			if (data)
			{
				Ref<Texture2D> result = Texture2D::Create(width, height);
				result->SetData(data, dataSize, channels);

				return result;
			}

			return nullptr;
		}

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