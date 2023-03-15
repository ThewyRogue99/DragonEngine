#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include <unordered_map>
#include <iostream>

namespace Engine
{
	class AssetMetadata
	{
	public:
		~AssetMetadata();

		MemoryMap& GetFields() { return FieldTable; }

		void Write(std::ostream& ss);
		void Read(std::istream& ss);

		static Ref<AssetMetadata> Create();

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
		MemoryMap FieldTable = { };
	};
}