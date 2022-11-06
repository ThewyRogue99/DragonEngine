#include "depch.h"
#include "Serializer.h"

#include <stb_image.h>

namespace Engine
{
	namespace Serializer
	{
		bool SerializeWithAssetType(AssetMetadata& metadata, std::istream& stream, AssetType type)
		{
			switch (type)
			{
			case AssetType::Texture:
				return SerializeTexture(metadata, stream);
			default:
				return false;
			}
		}

		bool SerializeTexture(AssetMetadata& metadata, std::istream& stream)
		{
			stream.seekg(0, std::ios::end);
			size_t buffSize = stream.tellg();
			stream.seekg(std::ios::beg);

			if (buffSize > 0)
			{
				stbi_set_flip_vertically_on_load(true);

				uint8_t* buff = new uint8_t[buffSize];

				stream.read((char*)buff, buffSize);

				int width, height, channels;
				unsigned char* data = stbi_load_from_memory(buff, (int)buffSize, &width, &height, &channels, 0);

				if (data)
				{
					int dataSize = width * height * channels;

					metadata.SetField("width", width);
					metadata.SetField("height", height);
					metadata.SetField("channels", channels);
					metadata.SetField("data", data, dataSize);

					stbi_image_free(data);

					return true;
				}

				delete[] buff;
			}

			return false;
		}

		Ref<Texture2D> DeserializeTexture(AssetMetadata& metadata)
		{
			int width, height, channels, dataSize;

			width = metadata.GetField<int>("width");
			height = metadata.GetField<int>("height");
			channels = metadata.GetField<int>("channels");

			dataSize = width * height * channels;

			uint8_t* data = (uint8_t*)metadata.GetField("data", dataSize);

			if (data)
			{
				Ref<Texture2D> result = Texture2D::Create(width, height);
				result->SetData(data, dataSize);

				return result;
			}

			return nullptr;
		}
	}
}