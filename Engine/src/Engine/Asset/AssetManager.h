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

	    static bool Load();
	    static bool Save();

        static Ref<Asset> CreateAsset(
            const CString& Path,
            const CString& Name,
            AssetType Type,
            Ref<AssetMetadata> Metadata = nullptr
        );

        static bool RenameAsset(Ref<Asset> AssetRef, const CString& NewName);

        static bool RemoveAsset(Ref<Asset> AssetRef);

        static bool MoveAsset(Ref<Asset> AssetRef, const CString& NewPath);

        static Ref<Asset> GetAsset(const std::string& AssetID);
        static Ref<Asset> GetAsset(const CString& Path, const CString& Name);

        static Ref<Asset> LoadAsset(const std::string& AssetID);
        static Ref<Asset> LoadAsset(const CString& Path, const CString& Name);

        static bool SaveAsset(Ref<Asset> AssetRef);

        static bool AssetExists(const CString& Path, const CString& Name);
        static bool AssetExists(const std::string& ID);

	    static CString GetFullPath(const CString& Path);
        static CString GetFullPath(const CString& Path, const CString& Name);

        static std::string GetAssetID(const CString& Path, const CString& Name);

	    static bool CreateFolder(const CString& Path, const CString& Name);
	    static bool RemoveFolder(const CString& Path, const CString& Name);
        static bool RenameFolder(const CString& Path, const CString& Name, const CString& NewName);

        static CString GetAvailableName(const CString& Path, const CString& Name);

        static AssetIterator GetIterator();

        friend class DirectoryEntry;
        friend class AssetIterator;

    private:
	    static bool LoadMetadata();
	    static bool SaveMetadata();

	    static bool AddAsset(const std::string& ID, AssetInfo* Info);

        using AssetMap = std::unordered_map<
            std::string,
            std::pair<
                AssetInfo*,
                WeakRef<Asset>
            >
        >;

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