#include "depch.h"
#include "AssetManager.h"

#include "Engine/Core/UUID.h"
#include "AssetMetadata.h"
#include "Serializer/Serializer.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

constexpr auto METADATA_FILE_NAME = L"metadata.dat";

namespace Engine
{
	static std::filesystem::path ContentPath = L"";
	static bool bIsAssetManagerInit = false;

	AssetManager::AssetMap AssetManager::AssetList = { };

	AssetIterator::AssetIterator(const CString& Path, bool IsFullPath)
	{
		EntryList = std::make_shared<EntryVector>();

		if (!IsFullPath && !bIsAssetManagerInit) return;

		CString FullPath = IsFullPath ?
			Path : AssetManager::GetFullPath(Path);

		if (std::filesystem::exists(FullPath))
		{
			for (auto& entry : std::filesystem::directory_iterator(FullPath))
			{
				DirectoryEntry DirEntry;

				bool isDirectory = entry.is_directory();
				if (isDirectory)
					DirEntry.Type = AssetType::Folder;

				if (!isDirectory)
				{
					std::filesystem::path fpath = entry.path();

					CString ext = fpath.extension();

					if (ext == L".deasset")
					{
						CString fname = fpath.filename();
						size_t pos = fname.find_last_of('.');
						CString id = fname.substr(0, pos);

						std::string c_id = TypeUtils::FromUTF16(id);

						DirEntry._it = AssetManager::AssetList.find(c_id);

						if (DirEntry._it == AssetManager::AssetList.end())
							continue;
					}
					else continue;
				}
				else
				{
					DirEntry.Path = Path;
					DirEntry.Name = entry.path().filename();

					std::filesystem::path fPath = DirEntry.Path;
					DirEntry.PathName = fPath / DirEntry.Name;
				}

				EntryList->push_back(DirEntry);
			}
		}

		_it = EntryList->begin();
	}

	const CString& DirectoryEntry::GetPath() const
	{
		if ((Type == AssetType::Folder))
			return Path;
		else
			return (*_it).second.Path;
	}

	const CString& DirectoryEntry::GetName() const
	{
		if ((Type == AssetType::Folder))
			return Name;
		else
			return (*_it).second.Name;
	}

	const CString& DirectoryEntry::GetPathName() const
	{
		if ((Type == AssetType::Folder))
			return PathName;
		else
			return (*_it).second.Path;
	}

	const std::string& DirectoryEntry::GetID() const
	{
		if (!(Type == AssetType::Folder))
			return (*_it).first;
		else
			throw std::out_of_range("Entry is not a Directory!");
	}

	const DirectoryEntry& AssetIterator::operator*() const
	{
		return *_it;
	}

	const DirectoryEntry* AssetIterator::operator->() const
	{
		return &(**this);
	}

	AssetIterator& AssetIterator::operator++()
	{
		++_it;

		return *this;
	}

	AssetIterator AssetIterator::operator++(int)
	{
		AssetIterator it = *this;
		++(*this);

		return it;
	}

	void AssetManager::Init(const CString& Path)
	{
		SetContentPath(Path);

		Load();

		bIsAssetManagerInit = true;
	}

	void AssetManager::SetContentPath(const CString& Path)
	{
		ContentPath = Path;
	}

	CString AssetManager::GetContentPath()
	{
		return ContentPath;
	}

	void AssetManager::Load()
	{
		LoadMetadata();
	}

	void AssetManager::Save()
	{
		SaveMetadata();
	}

	bool AssetManager::CreateAssetFromFile(const CString& TargetPath, const CString& DestinationPath, bool Overwrite)
	{
		if (bIsAssetManagerInit)
		{
			std::filesystem::path FPath = TargetPath;

			CString ext = FPath.extension();

			CString fname = FPath.filename();
			size_t idx = fname.find_last_of(L'.');

			CString name = fname.substr(0, idx);

			AssetType type = AssetUtils::GetAssetTypeFromExtension(ext);

			std::ifstream fi(FPath, std::ios::in | std::ios::binary);

			if (fi.is_open())
			{
				AssetMetadata data;

				if (Serializer::SerializeWithAssetType(data, fi, type))
				{
					std::string id = UUID().GetString();
					CString w_id = TypeUtils::FromUTF8(id);

					CString FullPath = (ContentPath / DestinationPath) / (w_id + L".deasset");

					if (!Overwrite)
						name = GetAvailableName(DestinationPath, name);

					std::fstream fo(FullPath, std::ios::out | std::ios::binary);
					if (fo.is_open())
					{
						Asset asset(name, id);
						asset.SetData(data, type);

						asset.Write(fo);

						fo.close();

						AssetList[id] = AssetData(name, DestinationPath, type);
						SaveMetadata();

						return true;
					}
				}
				fi.close();
			}
		}

		return false;
	}

	bool AssetManager::CreateAsset(const CString& Path, const CString& Name, AssetMetadata& Data, AssetType Type, bool Overwrite)
	{
		if (bIsAssetManagerInit)
		{
			std::string id;

			{
				bool exists = AssetExists(Path, Name);

				if (!Overwrite)
				{
					if (exists)
						return false;
					else
						id = UUID().GetString();
				}
				else if(exists)
					id = GetAssetID(Path, Name);
				else
					id = UUID().GetString();
			}

			CString w_id = TypeUtils::FromUTF8(id);

			CString FullPath = (ContentPath / Path) / (w_id + L".deasset");

			std::ofstream f(FullPath, std::ios::out | std::ios::binary);
			if (f.is_open())
			{
				AssetData data(Name, Path, Type);

				if (AddAsset(id, data))
				{
					if (!Data.Empty() && Type != AssetType::Undefined)
					{
						Asset asset(Name, id);
						asset.SetData(Data, Type);

						asset.Write(f);
					}

					SaveMetadata();
					return true;
				}

				f.close();
			}
		}

		return false;
	}

	bool AssetManager::RenameAsset(const std::string& AssetID, const CString& NewName)
	{
		if (!bIsAssetManagerInit) return false;

		try
		{
			AssetData& data = AssetManager::AssetList[AssetID];

			data.Name = NewName;

			SaveMetadata();
			return true;
		}
		catch (const std::out_of_range&)
		{
			return false;
		}
	}

	bool AssetManager::RenameAsset(const CString& Path, const CString& Name, const CString& NewName)
	{
		if (!bIsAssetManagerInit) return false;

		std::string id = GetAssetID(Path, Name);

		return RenameAsset(id, NewName);
	}

	bool AssetManager::RemoveAsset(const std::string& AssetID)
	{
		if (!bIsAssetManagerInit) return false;

		try
		{
			AssetData& data = AssetManager::AssetList[AssetID];

			CString w_id = TypeUtils::FromUTF8(AssetID);

			CString FullPath = (ContentPath / data.Path) / (w_id + L".deasset");

			AssetManager::AssetList.erase(AssetID);

			SaveMetadata();
			return std::filesystem::remove(FullPath);
		}
		catch (const std::out_of_range&)
		{
			return false;
		}
	}

	bool AssetManager::RemoveAsset(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return false;

		std::string id = GetAssetID(Path, Name);

		return RemoveAsset(id);
	}

	Asset AssetManager::LoadAsset(const std::string& AssetID)
	{
		if (!bIsAssetManagerInit) return Asset();

		try
		{
			AssetData& data = AssetList[AssetID];

			Asset asset(data.Name, AssetID);

			if (data.RefCount < 1)
			{
				CString w_id = TypeUtils::FromUTF8(AssetID);

				CString FullPath = (ContentPath / data.Path) / (w_id + L".deasset");

				std::ifstream f(FullPath, std::ios::in | std::ios::binary);
				if (f.is_open())
				{
					asset.SetData(data.Metadata, data.Type, false);

					asset.Read(f);

					asset.IncrementCounter = &AssetManager::IncrementCounterCallback;

					data.RefCount = 2;

					f.close();
				}
			}
			else
			{
				asset.Metadata = &data.Metadata;
				asset.IncrementCounter = &AssetManager::IncrementCounterCallback;

				data.RefCount += 2;
			}

			return asset;
		}
		catch (const std::out_of_range&)
		{
			return Asset();
		}
	}

	Asset AssetManager::LoadAsset(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return Asset();

		std::string id = GetAssetID(Path, Name);

		return LoadAsset(id);
	}

	CString AssetManager::GetFullPath(const CString& Path)
	{
		if (!bIsAssetManagerInit) return L"";

		return (ContentPath / Path);
	}

	CString AssetManager::GetFullPath(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return L"";

		CString path = (ContentPath / Path) / Name;
		return path + L".deasset";
	}

	bool AssetManager::AddAsset(const std::string& ID, const AssetData& data)
	{
		if (!bIsAssetManagerInit) return false;

		if (!AssetExists(data.Path, data.Name))
		{
			AssetList[ID] = data;
			return true;
		}

		return true;
	}

	void AssetManager::IncrementCounterCallback(std::string& ID)
	{
		if (!bIsAssetManagerInit) return;

		AssetData& data = AssetList[ID];

		int& Counter = data.RefCount;

		if (--Counter < 1)
		{
			data.Metadata.Clear();
			Counter = 0;
		}
	}

	bool AssetManager::CreateFolder(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return false;

		std::filesystem::path fullPath = ContentPath / Path;

		if(std::filesystem::exists(fullPath))
			return std::filesystem::create_directory(fullPath / Name);

		return false;
	}

	bool AssetManager::RemoveFolder(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return false;

		std::filesystem::path fullPath = (ContentPath / Path) / Name;

		if(std::filesystem::exists(fullPath))
			return std::filesystem::remove(fullPath);

		return false;
	}

	bool AssetManager::RenameFolder(const CString& Path, const CString& Name, const CString& NewName)
	{
		if (!bIsAssetManagerInit) return false;

		CString OldPath = (ContentPath / Path) / Name;

		if (std::filesystem::exists(OldPath))
		{
			CString NewPath = (ContentPath / Path) / NewName;

			std::filesystem::rename(OldPath, NewPath);

			return true;
		}

		return false;
	}

	AssetIterator AssetManager::GetIterator()
	{
		return bIsAssetManagerInit
			? AssetIterator(ContentPath, true) : AssetIterator(L"");
	}

	bool AssetManager::AssetExists(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return false;

		std::string id = GetAssetID(Path, Name);

		return !id.empty();
	}

	bool AssetManager::AssetExists(const std::string& ID)
	{
		if (!bIsAssetManagerInit) return false;

		auto it = AssetManager::AssetList.find(ID);

		return AssetManager::AssetList.end() != it;
	}

	CString AssetManager::GetAvailableName(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return L"";

		if (AssetExists(Path, Name))
		{
			int idx = 0;
			CString idx_str;

			do
			{
				if (idx > 50)
				{
					idx = -1;
					break;
				}

				idx_str = std::to_wstring(++idx);
			} while (AssetExists(Path, Name + idx_str));

			if (idx == -1)
				return L"";
			else
				return Name + idx_str;
		}

		return Name;
	}

	std::string AssetManager::GetAssetID(const CString& Path, const CString& Name)
	{
		if (!bIsAssetManagerInit) return "";

		for (auto& [key, value] : AssetManager::AssetList)
		{
			if (value.Name == Name && value.Path == Path)
				return key;
		}

		return "";
	}

	void AssetManager::LoadMetadata()
	{
		CString FullPath = ContentPath / METADATA_FILE_NAME;

		std::ifstream f(FullPath, std::ios::in | std::ios::binary);

		if (f.is_open())
		{
			AssetMetadata metadata;

			metadata.Read(f);

			AssetManager::AssetList.clear();

			for (auto& [id, data] : metadata)
			{
				AssetData assetData;
				assetData.Deserialize(*(AssetMetadata*)data.DataPtr);

				AssetList[id] = assetData;
			}

			f.close();
		}
	}

	void AssetManager::SaveMetadata()
	{
		CString FullPath = ContentPath / METADATA_FILE_NAME;

		std::ofstream f(FullPath, std::ios::out | std::ios::binary);

		if (f.is_open())
		{
			AssetMetadata metadata;

			for (auto& [id, data] : AssetList)
			{
				AssetMetadata serializeData;
				data.Serialize(serializeData);

				metadata.SetField(id, serializeData);
			}

			metadata.Write(f);

			f.close();
		}
	}

	void AssetManager::AssetData::Serialize(AssetMetadata& metadata)
	{
		metadata.SetStringField("Name", Name);
		metadata.SetStringField("Path", Path);
		metadata.SetField("Type", Type);
	}

	void AssetManager::AssetData::Deserialize(AssetMetadata& metadata)
	{
		Name = metadata.GetStringField<wchar_t>("Name");
		Path = metadata.GetStringField<wchar_t>("Path");
		Type = metadata.GetField<AssetType>("Type");
	}
}