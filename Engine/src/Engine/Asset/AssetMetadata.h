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

	private:
		class TableHeader
		{
		public:
			TableHeader() = default;
			TableHeader(const TableHeader&) = default;

			std::string fieldName;

			friend std::ostream& operator <<(std::ostream& ss, const TableHeader& header)
			{
				return TableHeader::WriteStream(ss, header);
			}

			friend std::istream& operator >>(std::istream& ss, TableHeader& header)
			{
				return TableHeader::ReadStream(ss, header);
			}

		private:
			static std::ostream& WriteStream(std::ostream& ss, const TableHeader& header);
			static std::istream& ReadStream(std::istream& ss, TableHeader& header);
		};

		class FieldHeader
		{
		public:
			FieldHeader() = default;
			FieldHeader(const FieldHeader&) = default;

			MemoryMap::FieldData Data;

			friend std::ostream& operator <<(std::ostream& ss, const FieldHeader& header)
			{
				return FieldHeader::WriteStream(ss, header);
			}

			friend std::istream& operator >>(std::istream& ss, FieldHeader& header)
			{
				return FieldHeader::ReadStream(ss, header);
			}

		private:
			static std::ostream& WriteStream(std::ostream& ss, const FieldHeader& header);
			static std::istream& ReadStream(std::istream& ss, FieldHeader& header);
		};

		struct phold {
			explicit phold(int) {}
		};

	public:
		explicit AssetMetadata(const phold&) : AssetMetadata() { }

	private:
		MemoryMap FieldTable = { };
	};
}