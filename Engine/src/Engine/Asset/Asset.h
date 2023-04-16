#pragma once
#include <iostream>
#include <functional>

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "AssetMetadata.h"
#include "AssetUtils.h"

namespace Engine
{
	class AssetInfo
	{
	public:
		AssetInfo() = default;
		AssetInfo(const AssetInfo&) = default;

		AssetInfo(
			WString Name,
			WString Path,
			CString ID,
			AssetType Type = AssetType::Undefined
		) : Name(Name), Path(Path), ID(ID), Type(Type) { }

		WString Name;
		WString Path;
		CString ID;

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

		ENGINE_API void Write(std::ostream& ss);
		ENGINE_API void Read(std::istream& ss);

	public:
		explicit Asset(const phold&) : Asset() { }

	private:
		AssetInfo* Info = nullptr;
	};
}