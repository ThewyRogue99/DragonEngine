#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "AssetMetadata.h"
#include "AssetUtils.h"

#include "Engine/Core/UUID.h"

#include <iostream>
#include <functional>

namespace Engine
{
	class Asset
	{
	public:
		const AssetMetadata* GetData() const { return Metadata; }

		const CString& GetName() const { return Name; }

		const std::string& GetID() const { return ID; }

		const AssetType GetAssetType() const { return Type; }

		friend class AssetManager;
		friend class AssetSerializer;

		Asset& operator=(Asset&) = delete;

	private:
		Asset(const CString& Name, UUID id = UUID()) : Name(Name), ID(id.GetString()) { }
		Asset(UUID id = UUID()) : ID(id.GetString()) { }

		Asset(const Asset&) = default;

		void Write(std::ostream& ss);
		void Read(std::istream& ss);

		void SetData(AssetMetadata* data, AssetType dataType = AssetType::Undefined, bool CopyData = true)
		{
			if (Metadata)
			{
				delete Metadata;
				Metadata = nullptr;
			}

			if (CopyData)
			{
				Metadata = new AssetMetadata();
				data->Copy(*Metadata);
			}
			else
				Metadata = data;

			if (dataType != AssetType::Undefined)
				Type = dataType;
		}

	private:
		AssetMetadata* Metadata = nullptr;
		
		CString Name;
		std::string ID;
		AssetType Type = AssetType::Undefined;
	};
}