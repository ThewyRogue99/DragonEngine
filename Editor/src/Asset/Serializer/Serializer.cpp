#include "depch.h"
#include "Serializer.h"

#include "../../Engine/vendor/stb_image/stb_image.h"
#include "Engine/Asset/AssetManager.h"
#include "Utils.h"

#include <filesystem>

namespace Engine
{
	namespace Serializer
	{
		bool SerializeWithAssetType(const CString& FilePath, AssetMetadata& metadata, AssetType type)
		{
			switch (type)
			{
			case AssetType::Texture:
				return SerializeTexture(FilePath, metadata);
			case AssetType::Audio:
				return SerializeAudio(FilePath, metadata);
			default:
				return false;
			}
		}

		bool SerializeTexture(const CString& FilePath, AssetMetadata& metadata)
		{
			stbi_set_flip_vertically_on_load(true);

			FILE* f = _wfopen(FilePath.c_str(), L"rb");

			int width, height, channels;
			unsigned char* data = stbi_load_from_file(f, &width, &height, &channels, 0);
			fclose(f);

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

			return false;
		}

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

		bool SerializeAudio(const CString& FilePath, AssetMetadata& metadata)
		{
			AudioToWav::Audio res = AudioToWav::Utils::LoadAudioFile(FilePath);
			
			if (!res.Empty())
			{
				int Channels = res.GetNumChannels();
				int SampleSize = res.GetSampleSize();
				uint32_t SampleRate = res.GetSampleRate();

				const void* Data = res.GetData();
				size_t DataSize = res.GetDataSize();

				metadata.SetField("Channels", Channels);
				metadata.SetField("SampleSize", SampleSize);
				metadata.SetField("SampleRate", SampleRate);
				metadata.SetField("Data", (void*)Data, DataSize);

				res.Clear();

				return true;
			}

			return false;
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

		bool CreateAssetFromFile(const CString& TargetPath, const CString& DestinationPath, bool Overwrite)
		{
			std::filesystem::path FPath = TargetPath;

			CString ext = FPath.extension();

			CString fname = FPath.filename();
			size_t idx = fname.find_last_of(L'.');

			CString name = fname.substr(0, idx);

			AssetType type = AssetUtils::GetAssetTypeFromExtension(ext);

			AssetMetadata data;

			if (Serializer::SerializeWithAssetType(FPath, data, type))
			{
				std::string id = UUID().GetString();
				CString w_id = TypeUtils::FromUTF8(id);

				std::filesystem::path ContentPath = AssetManager::GetContentPath();
				CString FullPath = (ContentPath / DestinationPath) / (w_id + L".deasset");

				if (!Overwrite)
					name = AssetManager::GetAvailableName(DestinationPath, name);

				AssetManager::CreateAsset(DestinationPath, name, data, type, Overwrite);

				return true;
			}

			return false;
		}
	}
}