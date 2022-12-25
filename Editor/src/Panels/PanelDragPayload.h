#pragma once

#include "Engine/Types/Types.h"
#include "Engine/Asset/AssetUtils.h"

namespace Engine
{
	namespace PanelDragPayload
	{
		class ContentBrowserItem
		{
		public:
			ContentBrowserItem() = default;

			const char* GetID() const { return ID; }
			const wchar_t* GetPath() const { return Path; }
			const wchar_t* GetName() const { return Name; }

			uint8_t* Data(size_t& DataSize)
			{
				if (ItemType == AssetType::Folder)
				{
					size_t PathSize = (wcslen(Path) + 1) * 2;
					size_t NameSize = (wcslen(Name) + 1) * 2;

					DataSize = sizeof(ItemType) + PathSize + NameSize;
					uint8_t* DataPtr = new uint8_t[DataSize];

					memcpy(DataPtr, &ItemType, sizeof(ItemType));
					wcscpy((wchar_t*)(DataPtr + sizeof(ItemType)), Path);
					wcscpy((wchar_t*)(DataPtr + sizeof(ItemType) + PathSize), Name);

					return DataPtr;
				}
				else
				{
					size_t IDlen = strlen(ID);

					DataSize = sizeof(ItemType) + (IDlen + 1);
					uint8_t* DataPtr = new uint8_t[DataSize];
				
					memcpy(DataPtr, &ItemType, sizeof(ItemType));
					strcpy((char*)DataPtr + sizeof(ItemType), ID);

					return DataPtr;
				}
			}

			void FromData(uint8_t* Data)
			{
				memcpy(&ItemType, Data, sizeof(ItemType));
				
				if (ItemType == AssetType::Folder)
				{
					Path = (wchar_t*)(Data + sizeof(ItemType));

					size_t PathSize = (wcslen(Path) + 1)* 2;

					Name = (wchar_t*)(Data + sizeof(ItemType)) + PathSize;
				}
				else
				{
					ID = (char*)Data + sizeof(ItemType);
				}
			}

			void SetAsset(const char* id)
			{
				ID = id;
			}

			void SetFolder(const wchar_t* path, const wchar_t* name)
			{
				ItemType = AssetType::Folder;

				Path = path;
				Name = name;
			}

			AssetType ItemType = AssetType::Undefined;

		private:
			const char* ID = nullptr;

			const wchar_t* Path = nullptr;
			const wchar_t* Name = nullptr;
		};
	}
}