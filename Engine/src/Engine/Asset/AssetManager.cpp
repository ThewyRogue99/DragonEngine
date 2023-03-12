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
			return _it->second.first->Path;
	}

	const CString& DirectoryEntry::GetName() const
	{
		if (dFolder)
			return dFolder->Name;
		else
			return _it->second.first->Name;
	}

	const CString& DirectoryEntry::GetPathName() const
	{
		if (dFolder)
			return dFolder->PathName;
		else
			return _it->second.first->Path;
	}

	const std::string& DirectoryEntry::GetID() const
	{
		if (!dFolder)
			return _it->first;
		else
			throw std::out_of_range("Cannot get the ID of a Directory!");
	}

	const AssetType DirectoryEntry::GetType() const
	{
		if (dFolder)
			return AssetType::Folder;
		else
			return _it->second.first->Type;
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

	Ref<Asset> AssetManager::CreateAsset(const CString& Path, const CString& Name, AssetType Type, Ref<AssetMetadata> Metadata)
	{
		if (bIsAssetManagerInit)
		{
			if (AssetExists(Path, Name))
			{
				DE_ERROR(AssetManager, "Cannot create an asset that already exists!");
				return nullptr;
			}

			std::string id = UUID().GetString();

			CString w_id = TypeUtils::FromUTF8(id);

			CString FullPath = (ContentPath / Path) / (w_id + L".deasset");

			std::ofstream f(FullPath, std::ios::out | std::ios::binary);
			if (f.is_open())
			{
				std::string cPath = TypeUtils::FromUTF16(Path);
				DE_LOG(AssetManager, "Creating Asset in {0}", cPath.c_str());

				AssetInfo* info = new AssetInfo(Name, Path, id, Type);

				if (AddAsset(id, info))
				{
					Ref<Asset> AssetRef = CreateRef<Asset>(Asset::phold{ 0 });
					AssetRef->Info = info;
					AssetRef->Metadata = Metadata;
					AssetRef->Write(f);

					AssetList[id].second = AssetRef;

					SaveMetadata();
					return AssetRef;
				}

				f.close();
			}
		}

		return false;
	}

	bool AssetManager::RenameAsset(Ref<Asset> AssetRef, const CString& NewName)
	{
		if (AssetRef && bIsAssetManagerInit)
		{
			auto it = AssetList.find(AssetRef->GetInfo().ID);
			if (it != AssetList.end())
			{
				it->second.first->Name = NewName;

				SaveMetadata();
				return true;
			}
		}

		return false;
	}

	bool AssetManager::RemoveAsset(Ref<Asset> AssetRef)
	{
		if (AssetRef && bIsAssetManagerInit)
		{
			const std::string& AssetID = AssetRef->GetInfo().ID;

			auto it = AssetList.find(AssetID);
			if (it != AssetList.end())
			{
				CString w_id = TypeUtils::FromUTF8(AssetID);

				CString FullPath = (ContentPath / AssetRef->GetInfo().Path) / (w_id + L".deasset");

				AssetManager::AssetList.erase(AssetID);

				SaveMetadata();
				return std::filesystem::remove(FullPath);
			}
		}

		return false;
	}

	bool AssetManager::MoveAsset(Ref<Asset> AssetRef, const CString& NewPath)
	{
		if (AssetRef && bIsAssetManagerInit)
		{
			const std::string& AssetID = AssetRef->GetInfo().ID;

			AssetInfo* info = AssetList[AssetID].first;

			if (info->Type != AssetType::Folder)
			{
				std::filesystem::path p_OldPath = ContentPath / info->Path;
				p_OldPath /= AssetID + ".deasset";

				std::filesystem::path p_NewPath = ContentPath / NewPath;
				p_NewPath /= AssetID + ".deasset";

				std::filesystem::rename(p_OldPath, p_NewPath);

				info->Path = NewPath;
				SaveMetadata();

				return true;
			}
		}

		return false;
	}

	Ref<Asset> AssetManager::GetAsset(const std::string& AssetID)
	{
		auto& it = AssetList.find(AssetID);
		if (it != AssetList.end())
		{
			Ref<Asset> AssetRef = CreateRef<Asset>(Asset::phold{ 0 });
			AssetRef->Info = it->second.first;

			it->second.second = AssetRef;

			return AssetRef;
		}

		return nullptr;
	}

	Ref<Asset> AssetManager::GetAsset(const CString& Path, const CString& Name)
	{
		return GetAsset(GetAssetID(Path, Name));
	}

	Ref<Asset> AssetManager::LoadAsset(const std::string& AssetID)
	{
		if (!bIsAssetManagerInit) return nullptr;

		auto& it = AssetList.find(AssetID);
		if (it != AssetList.end())
		{
			WeakRef<Asset>& WeakAsset = it->second.second;

			// Check if an asset is already loaded
			if (WeakAsset.expired())
			{
				AssetInfo* info = it->second.first;

				CString w_id = TypeUtils::FromUTF8(AssetID);
				CString FullPath = (ContentPath / info->Path) / (w_id + L".deasset");

				std::ifstream f(FullPath, std::ios::in | std::ios::binary);
				if (f.is_open())
				{
					Ref<Asset> AssetRef = CreateRef<Asset>(Asset::phold{ 0 });
					AssetRef->Info = info;
					AssetRef->Metadata = AssetMetadata::Create();
					AssetRef->Read(f);

					WeakAsset = AssetRef;

					f.close();

					return AssetRef;
				}
			}
			else
			{
				Ref<Asset> AssetRef = WeakAsset.lock();

				if (AssetRef->IsLoaded())
				{
					return AssetRef;
				}
				else
				{
					CString w_id = TypeUtils::FromUTF8(AssetID);

					CString FullPath = (ContentPath / AssetRef->GetInfo().Path) / (w_id + L".deasset");

					std::ifstream f(FullPath, std::ios::in | std::ios::binary);
					if (f.is_open())
					{
						AssetRef->Metadata = AssetMetadata::Create();
						AssetRef->Read(f);

						f.close();

						return AssetRef;
					}
				}
			}
		}

		return nullptr;
	}

	Ref<Asset> AssetManager::LoadAsset(const CString& Path, const CString& Name)
	{
		std::string id = GetAssetID(Path, Name);

		return LoadAsset(id);
	}

	bool AssetManager::SaveAsset(Ref<Asset> AssetRef)
	{
		const std::string& AssetID = AssetRef->GetInfo().ID;

		CString w_id = TypeUtils::FromUTF8(AssetID);

		CString FullPath = (ContentPath / AssetRef->GetInfo().Path) / (w_id + L".deasset");

		std::ofstream f(FullPath, std::ios::out | std::ios::binary);
		if (f.is_open())
		{
			AssetRef->Write(f);

			f.close();

			return true;
		}

		return false;
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

	bool AssetManager::AddAsset(const std::string& ID, AssetInfo* Info)
	{
		if (bIsAssetManagerInit)
		{
			if (!AssetExists(Info->Path, Info->Name))
			{
				AssetList[ID].first = Info;
				return true;
			}
		}

		return false;
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
		if (bIsAssetManagerInit)
		{
			for (auto& [key, pair] : AssetManager::AssetList)
			{
				if (pair.first->Name == Name && pair.first->Path == Path)
					return key;
			}
		}

		return "";
	}

	bool AssetManager::LoadMetadata()
	{
		CString FullPath = ContentPath / METADATA_FILE_NAME;

		std::ifstream f(FullPath, std::ios::in | std::ios::binary);

		if (f.is_open())
		{
			AssetList.clear();

			Ref<AssetMetadata> metadata = AssetMetadata::Create();

			metadata->Read(f);

			for (auto& [id, data] : metadata->GetFields())
			{
				if (data.IsMap)
				{
					AssetInfo* Info = new AssetInfo();
					MemoryMap& AssetMap = *((MemoryMap*)data.DataPtr);

					Info->Name = AssetMap.GetStringField<wchar_t>("Name");
					Info->Path = AssetMap.GetStringField<wchar_t>("Path");
					Info->Type = AssetMap.GetField<AssetType>("Type");
					Info->ID = id;

					AssetList[id].first = Info;
				}
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
			Ref<AssetMetadata> Metadata = AssetMetadata::Create();
			MemoryMap& MetadataMap = Metadata->GetFields();

			for (auto& [id, data] : AssetList)
			{
				MemoryMap SerializeMap;

				SerializeMap.SetStringField("Name", data.first->Name);
				SerializeMap.SetStringField("Path", data.first->Path);
				SerializeMap.SetField("Type", data.first->Type);

				MetadataMap.SetField(id, SerializeMap);
			}

			Metadata->Write(f);

			f.close();

			return true;
		}

		return false;
	}
}