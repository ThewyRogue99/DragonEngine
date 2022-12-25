#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Asset/Asset.h"

#include <fstream>
#include <unordered_map>

namespace Engine
{
    class AssetIterator;

    class AssetManager
    {
    public:
        static void Init(const CString& Path);

	    static void SetContentPath(const CString& Path);
	    static CString GetContentPath();

	    static void Load();
	    static void Save();

        static bool CreateAsset(const CString& Path, const CString& Name, AssetMetadata& Data, AssetType Type, bool Overwrite = false);

        static bool RenameAsset(const std::string& AssetID, const CString& NewName);
        static bool RenameAsset(const CString& Path, const CString& Name, const CString& NewName);

        static bool RemoveAsset(const std::string& AssetID);
	    static bool RemoveAsset(const CString& Path, const CString& Name);

        static void CloseAsset(Asset& asset);

        static Asset LoadAsset(const std::string& AssetID);
        static Asset LoadAsset(const CString& Path, const CString& Name);

	    static CString GetFullPath(const CString& Path);
        static CString GetFullPath(const CString& Path, const CString& Name);

	    static bool CreateFolder(const CString& Path, const CString& Name);
	    static bool RemoveFolder(const CString& Path, const CString& Name);
        static bool RenameFolder(const CString& Path, const CString& Name, const CString& NewName);

        static CString GetAvailableName(const CString& Path, const CString& Name);

        static AssetIterator GetIterator();

        friend class DirectoryEntry;
        friend class AssetIterator;

    private:
        class AssetData
        {
        public:
            AssetData() = default;
            AssetData(CString Name, CString Path, AssetType Type)
                : Name(Name), Path(Path), Type(Type) { }

            CString Name;
            CString Path;

            AssetType Type = AssetType::Undefined;

            AssetMetadata Metadata;

            void Serialize(AssetMetadata& metadata);
            void Deserialize(AssetMetadata& metadata);
        };

	    static void LoadMetadata();
	    static void SaveMetadata();

	    static bool AssetExists(const CString& Path, const CString& Name);
        static bool AssetExists(const std::string& ID);

        static std::string GetAssetID(const CString& Path, const CString& Name);

	    static bool AddAsset(const std::string& ID, const AssetData& data);

        static void IncrementCounterCallback(std::string& ID);

        using AssetMap = std::unordered_map<std::string, AssetData>;

        static AssetMap AssetList;
    };

    class DirectoryEntry
    {
    public:
        DirectoryEntry() = default;
        DirectoryEntry(const DirectoryEntry&) = default;

        friend class AssetManager;
        friend class AssetIterator;

    public:
        const CString& GetPath() const;
        const CString& GetName() const;
        const CString& GetPathName() const;

        const std::string& GetID() const;

        const AssetType GetType() const;

    private:
        struct FolderData
        {
            CString Path;
            CString Name;

            CString PathName;
        };
        Ref<FolderData> dFolder = nullptr;

        AssetManager::AssetMap::iterator _it = AssetManager::AssetList.end();
    };

    class AssetIterator
    {
    public:
        AssetIterator() = default;
        AssetIterator(const AssetIterator&) = default;
        AssetIterator(const CString& Path, bool IsFullPath = false);

        const DirectoryEntry& operator*() const;

        const DirectoryEntry* operator->() const;

        AssetIterator& operator++();
        AssetIterator operator++(int);

        bool operator==(const AssetIterator& _Rhs) const
        {
            return _it == _Rhs._it;
        }

    #if !_HAS_CXX20
        _NODISCARD bool operator!=(const AssetIterator& _Rhs) const
        {
            return _it != _Rhs._it;
        }
    #endif // !_HAS_CXX20

        AssetIterator begin()
        {
            AssetIterator it;
            it.EntryList = EntryList;
            it._it = EntryList->begin();

            return it;
        }

        AssetIterator end()
        {
            AssetIterator it;
            it.EntryList = EntryList;
            it._it = EntryList->end();

            return it;
        }

    private:
        using EntryVector = std::vector<DirectoryEntry>;

    private:
        EntryVector::iterator _it;

        Ref<EntryVector> EntryList = nullptr;
    };
}