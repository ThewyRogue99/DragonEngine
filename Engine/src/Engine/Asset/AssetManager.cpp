#include "depch.h"
#include "AssetManager.h"

#include "Engine/Core/UUID.h"
#include "AssetMetadata.h"

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
					DirEntry.dFolder = CreateRef<DirectoryEntry::FolderData>();
					DirEntry.dFolder->Path = Path;
					DirEntry.dFolder->Name = entry.path().filename();

					std::filesystem::path fPath = DirEntry.dFolder->Path;
					DirEntry.dFolder->PathName = fPath / DirEntry.dFolder->Name;
				}

				EntryList->push_back(DirEntry);
			}
		}

		_it = EntryList->begin();
	}

	const CString& DirectoryEntry::GetPath() const
	{
		if (dFolder)
			return dFolder->Path;
		else
			return (*_it).second.Path;
	}

	const CString& DirectoryEntry::GetName() const
	{
		if (dFolder)
			return dFolder->Name;
		else
			return (*_it).second.Name;
	}

	const CString& DirectoryEntry::GetPathName() const
	{
		if (dFolder)
			return dFolder->PathName;
		else
			return (*_it).second.Path;
	}

	const std::string& DirectoryEntry::GetID() const
	{
		if (!dFolder)
			return (*_it).first;
		else
			throw std::out_of_range("Entry is not a Directory!");
	}

	const AssetType DirectoryEntry::GetType() const
	{
		if (dFolder)
			return AssetType::Folder;
		else
			return (*_it).second.Type;
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
		DE_LOG(AssetManager, "Initializing AssetManager...");

		SetContentPath(Path);

		bIsAssetManagerInit = Load();

		if (bIsAssetManagerInit)
			DE_INFO(AssetManager, "Successfully initialized AssetManager");
		else
			DE_ERROR(AssetManager, "Failed to initialize AssetManager");
	}

	void AssetManager::SetContentPath(const CString& Path)
	{
		ContentPath = Path;
	}

	CString AssetManager::GetContentPath()
	{
		return ContentPath;
	}

	bool AssetManager::Load()
	{
		return LoadMetadata();
	}

	bool AssetManager::Save()
	{
		return SaveMetadata();
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
				DE_LOG(AssetManager, "Creating Asset in");

				AssetData data(Name, Path, Type);

				if (AddAsset(id, data))
				{
					if (!Data.Empty() && Type != AssetType::Undefined)
					{
						Asset asset(Name, id);
						Data.Copy(asset.Metadata);
						asset.Type = Type;

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

	bool AssetManager::MoveAsset(const std::string& AssetID, const CString& NewPath)
	{
		AssetData& data = AssetList[AssetID];

		if (data.Type != AssetType::Folder)
		{
			std::filesystem::path p_OldPath = ContentPath / data.Path;
			p_OldPath /= AssetID + ".deasset";

			std::filesystem::path p_NewPath = ContentPath / NewPath;
			p_NewPath /= AssetID + ".deasset";

			std::filesystem::rename(p_OldPath, p_NewPath);

			data.Path = NewPath;
			SaveMetadata();

			return true;
		}

		return false;
	}

	bool AssetManager::MoveAsset(const CString& Path, const CString& Name, const CString& NewPath)
	{
		if (!bIsAssetManagerInit) return false;

		std::string id = GetAssetID(Path, Name);

		return MoveAsset(id, NewPath);
	}

	Ref<Asset> AssetManager::LoadAsset(const std::string& AssetID)
	{
		if (!bIsAssetManagerInit) return nullptr;

		auto& it = AssetList.find(AssetID);
		if (it != AssetList.end())
		{
			AssetData& data = it->second;

			// Check if an asset is already loaded
			if (data.AssetRef.expired())
			{
				// Asset is not loaded
				CString w_id = TypeUtils::FromUTF8(AssetID);

				CString FullPath = (ContentPath / data.Path) / (w_id + L".deasset");

				std::ifstream f(FullPath, std::ios::in | std::ios::binary);
				if (f.is_open())
				{
					Ref<Asset> asset = CreateRef<Asset>(Asset::phold{ 0 }, data.Name, AssetID);
					data.AssetRef = asset;

					asset->Type = data.Type;

					asset->Read(f);

					f.close();

					return asset;
				}

				return nullptr;
			}
			else
			{
				// Asset is loaded
				return data.AssetRef.lock();
			}
		}

		return nullptr;
	}

	Ref<Asset> AssetManager::LoadAsset(const CString& Path, const CString& Name)
	{
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
				return Name + L' ' + idx_str;
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

	bool AssetManager::LoadMetadata()
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

			return true;
		}

		return false;
	}

	bool AssetManager::SaveMetadata()
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

			return true;
		}

		return false;
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