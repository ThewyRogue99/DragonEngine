#pragma once
#include <unordered_map>
#include <iostream>

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

namespace Engine
{
	class AssetMetadata
	{
	public:
		ENGINE_API ~AssetMetadata();

		ENGINE_API MemoryMap& GetFields() { return FieldTable; }

		ENGINE_API void Write(std::ostream& ss);
		ENGINE_API void Read(std::istream& ss);

		ENGINE_API static Ref<AssetMetadata> Create();

		AssetMetadata& operator=(const AssetMetadata&) = delete;

	private:
		AssetMetadata() = default;
		AssetMetadata(const AssetMetadata&) = default;

		struct phold {
			explicit phold(int) {}
		};

	public:
		explicit AssetMetadata(const phold&) : AssetMetadata() { }

	private:
		MemoryMap FieldTable;
	};
}