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

			void SetID(const std::string& id) { ID = id; }
			const std::string& GetID() const { return ID; }

			uint8_t* Data(size_t& DataSize)
			{
				DataSize = sizeof(ItemType) + (ID.size() + 1);
				uint8_t* DataPtr = new uint8_t[DataSize];
				
				memcpy(DataPtr, &ItemType, sizeof(ItemType));
				strcpy((char*)DataPtr + sizeof(ItemType), ID.c_str());

				return DataPtr;
			}

			void FromData(uint8_t* Data)
			{
				memcpy(&ItemType, Data, sizeof(ItemType));
				
				ID = (char*)Data + sizeof(ItemType);
			}

			AssetType ItemType;

		private:
			std::string ID;
		};
	}
}