#include "depch.h"
#include "AssetMetadata.h"

namespace Engine
{
	AssetMetadata::~AssetMetadata()
	{
		FieldTable.Clear();
	}

	void AssetMetadata::Write(std::ostream& ss)
	{
		// Write Field Table

		uint32_t size = (uint32_t)FieldTable.size();
		ss.write((char*)&size, sizeof(size));

		for (auto& pair : FieldTable)
		{
			TableHeader h;
			h.fieldName = pair.first;

			ss << h;
		}

		// Write Field Values

		for (auto& pair : FieldTable)
		{
			MemoryMap::FieldData& data = pair.second;

			FieldHeader h;
			h.Data.DataSize = data.DataSize;
			h.Data.IsMap = data.IsMap;
			h.Data.DataPtr = data.DataPtr;

			ss << h;
		}
	}

	void AssetMetadata::Read(std::istream& ss)
	{
		uint32_t size = 0;
		ss.read((char*)&size, sizeof(size));

		if (size > 0)
		{
			// Read Table Header Section
			std::vector<TableHeader> TableHeaderList(size);

			for (uint32_t i = 0; i < size; i++)
			{
				TableHeader h;
				ss >> h;

				TableHeaderList[i] = h;
			}

			// Read Field Section
			std::vector<FieldHeader> FieldHeaderList(size);

			for (uint32_t i = 0; i < size; i++)
			{
				FieldHeader h;
				ss >> h;

				FieldHeaderList[i] = h;
			}

			// Set Field Table
			for (uint32_t i = 0; i < size; i++)
			{
				std::string FieldName = TableHeaderList[i].fieldName;

				MemoryMap::FieldData& data = FieldTable.GetFieldData(FieldName);

				data.DataSize = FieldHeaderList[i].Data.DataSize;
				data.IsMap = FieldHeaderList[i].Data.IsMap;
				data.DataPtr = FieldHeaderList[i].Data.DataPtr;
			}
		}
	}

	Ref<AssetMetadata> AssetMetadata::Create()
	{
		return CreateRef<AssetMetadata>(phold{ 0 });
	}

	std::ostream& AssetMetadata::TableHeader::WriteStream(std::ostream& ss, const TableHeader& header)
	{
		uint32_t nameSize = (uint32_t)(header.fieldName.size() + 1);

		ss.write((char*)&nameSize, sizeof(uint32_t));

		ss.write(header.fieldName.c_str(), nameSize);

		return ss;
	}

	std::istream& AssetMetadata::TableHeader::ReadStream(std::istream& ss, TableHeader& header)
	{
		uint32_t nameSize = 0;
		ss.read((char*)&nameSize, sizeof(uint32_t));

		char* buff = new char[nameSize];
		ss.read(buff, nameSize);

		header.fieldName = buff;
		delete[] buff;

		return ss;
	}

	std::ostream& AssetMetadata::FieldHeader::WriteStream(std::ostream& ss, const FieldHeader& header)
	{
		if (header.Data.DataPtr)
		{
			ss.write((char*)&header.Data.DataSize, sizeof(uint32_t));
			ss.write((char*)&header.Data.IsMap, sizeof(bool));

			if (header.Data.IsMap)
			{
				AssetMetadata* metadata = (AssetMetadata*)header.Data.DataPtr;
				metadata->Write(ss);
			}
			else
			{
				ss.write((char*)header.Data.DataPtr, header.Data.DataSize);
			}
		}

		return ss;
	}

	std::istream& AssetMetadata::FieldHeader::ReadStream(std::istream& ss, FieldHeader& header)
	{
		uint32_t DataSize = 0;
		ss.read((char*)&DataSize, sizeof(uint32_t));

		bool IsMap = false;
		ss.read((char*)&IsMap, sizeof(bool));

		if (IsMap)
		{
			AssetMetadata* metadata = new AssetMetadata();
			metadata->Read(ss);

			header.Data.DataSize = DataSize;
			header.Data.IsMap = true;
			header.Data.DataPtr = metadata;
		}
		else
		{
			if (DataSize > 0)
			{
				char* buff = new char[DataSize];
				ss.read(buff, DataSize);

				header.Data.DataSize = DataSize;
				header.Data.IsMap = false;
				header.Data.DataPtr = buff;
			}
		}

		return ss;
	}
}