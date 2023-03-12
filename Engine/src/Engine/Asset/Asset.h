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
	class AssetInfo
	{
	public:
		AssetInfo() = default;
		AssetInfo(const AssetInfo&) = default;

		AssetInfo(
			CString Name,
			CString Path,
			std::string ID,
			AssetType Type = AssetType::Undefined
		) : Name(Name), Path(Path), ID(ID), Type(Type) { }

		CString Name;
		CString Path;
		std::string ID;

		AssetType Type = AssetType::Undefined;
	};

	class Asset
	{
	public:
		const AssetInfo& GetInfo() const { return *Info; }

		bool IsLoaded() const { return Metadata != nullptr; }

		friend class AssetManager;
		friend class AssetSerializer;

		Asset& operator=(Asset&) = delete;

		Ref<AssetMetadata> Metadata = nullptr;

	private:
		struct phold {
			explicit phold(int) {}
		};

		Asset() = default;
		Asset(const Asset&) = default;

		void Write(std::ostream& ss);
		void Read(std::istream& ss);

	public:
		explicit Asset(const phold&) : Asset() { }

	private:
		AssetInfo* Info = nullptr;
	};
}