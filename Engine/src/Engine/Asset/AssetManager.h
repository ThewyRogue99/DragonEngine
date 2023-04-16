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
        ENGINE_API static void Init(const WString& Path);

        ENGINE_API static void SetContentPath(const WString& Path);
        ENGINE_API static WString GetContentPath();

        ENGINE_API static bool Load();
        ENGINE_API static bool Save();

        ENGINE_API static Ref<Asset> CreateAsset(
            const WString& Path,
            const WString& Name,
            AssetType Type,
            Ref<AssetMetadata> Metadata = nullptr
        );

        ENGINE_API static bool RenameAsset(Ref<Asset> AssetRef, const WString& NewName);

        ENGINE_API static bool RemoveAsset(Ref<Asset> AssetRef);

        ENGINE_API static bool MoveAsset(Ref<Asset> AssetRef, const WString& NewPath);

        ENGINE_API static Ref<Asset> GetAsset(const CString& AssetID);
        ENGINE_API static Ref<Asset> GetAsset(const WString& Path, const WString& Name);

        ENGINE_API static Ref<Asset> LoadAsset(const CString& AssetID);
        ENGINE_API static Ref<Asset> LoadAsset(const WString& Path, const WString& Name);

        ENGINE_API static bool SaveAsset(Ref<Asset> AssetRef);

        ENGINE_API static bool AssetExists(const WString& Path, const WString& Name);
        ENGINE_API static bool AssetExists(const CString& ID);

        ENGINE_API static WString GetFullPath(const WString& Path);
        ENGINE_API static WString GetFullPath(const WString& Path, const WString& Name);

        ENGINE_API static CString GetAssetID(const WString& Path, const WString& Name);

        ENGINE_API static bool CreateFolder(const WString& Path, const WString& Name);
        ENGINE_API static bool RemoveFolder(const WString& Path, const WString& Name);
        ENGINE_API static bool RenameFolder(const WString& Path, const WString& Name, const WString& NewName);

        ENGINE_API static WString GetAvailableName(const WString& Path, const WString& Name);

        ENGINE_API static AssetIterator GetIterator();

        friend class DirectoryEntry;
        friend class AssetIterator;

    private:
        ENGINE_API static bool LoadMetadata();
        ENGINE_API static bool SaveMetadata();

        ENGINE_API static bool AddAsset(const CString& ID, AssetInfo* Info);

        using AssetMap = std::unordered_map<
            CString,
            std::pair<
            AssetInfo*,
            WeakRef<Asset>
            >
        >;

        ENGINE_API static AssetMap AssetList;
    };

    class DirectoryEntry
    {
    public:
        DirectoryEntry() = default;
        DirectoryEntry(const DirectoryEntry&) = default;

        friend class AssetManager;
        friend class AssetIterator;

    public:
        ENGINE_API const WString& GetPath() const;
        ENGINE_API const WString& GetName() const;
        ENGINE_API const WString& GetPathName() const;

        ENGINE_API const CString& GetID() const;

        ENGINE_API const AssetType GetType() const;

    private:
        struct FolderData
        {
            WString Path;
            WString Name;

            WString PathName;
        };
        Ref<FolderData> dFolder = nullptr;

        AssetManager::AssetMap::iterator _it = AssetManager::AssetList.end();
    };

    class AssetIterator
    {
    public:
        AssetIterator() = default;
        AssetIterator(const AssetIterator&) = default;
        ENGINE_API AssetIterator(const WString& Path, bool IsFullPath = false);

        ENGINE_API const DirectoryEntry& operator*() const;

        ENGINE_API const DirectoryEntry* operator->() const;

        ENGINE_API AssetIterator& operator++();
        ENGINE_API AssetIterator operator++(int);

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

        ENGINE_API AssetIterator begin();

        ENGINE_API AssetIterator end();

    private:
        using EntryVector = std::vector<DirectoryEntry>;

    private:
        EntryVector::iterator _it;

        Ref<EntryVector> EntryList = nullptr;
    };
}