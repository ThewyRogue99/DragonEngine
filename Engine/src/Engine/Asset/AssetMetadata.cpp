#include "depch.h"
#include "AssetMetadata.h"

namespace Engine
{
	AssetMetadata::~AssetMetadata()
	{
		Clear();
	}

	void AssetMetadata::SetField(const std::string& field, void* buff, size_t buffsize)
	{
		uint8_t* databuff = new uint8_t[buffsize];

		memcpy(databuff, buff, buffsize);

		if (FieldExists(field))
		{
			FieldData& data = FieldTable[field];

			if (data.DataPtr)
			{
				if (data.IsMap)
					delete (AssetMetadata*)data.DataPtr;
				else
					delete[] data.DataPtr;
			}

			data.DataPtr = databuff;
			data.DataSize = (uint32_t)buffsize;
		}
		else
		{
			FieldTable[field] = { (uint32_t)buffsize, false, databuff };
		}
	}

	void AssetMetadata::SetField(const std::string& field, AssetMetadata& data)
	{
		AssetMetadata* copyData = new AssetMetadata();
		data.Copy(*copyData);

		if (FieldExists(field))
		{
			FieldData& data = FieldTable[field];

			if (data.DataPtr)
			{
				if (data.IsMap)
					delete (AssetMetadata*)data.DataPtr;
				else
					delete[] data.DataPtr;
			}

			data.DataPtr = copyData;
			data.DataSize = 0;
		}
		else
		{
			FieldTable[field] = { (uint32_t)(data.FieldTable.size()), true, copyData};
		}
	}

	struct TableHeader
	{
		std::string fieldName;

		friend std::ostream& operator <<(std::ostream& ss, const TableHeader& header)
		{
			uint32_t nameSize = (uint32_t)(header.fieldName.size() + 1);

			ss.write((char*)&nameSize, sizeof(uint32_t));

			ss.write(header.fieldName.c_str(), nameSize);

			return ss;
		}

		friend std::istream& operator >>(std::istream& ss, TableHeader& header)
		{
			uint32_t nameSize = 0;
			ss.read((char*)&nameSize, sizeof(uint32_t));

			char* buff = new char[nameSize];
			ss.read(buff, nameSize);

			header.fieldName = buff;
			delete[] buff;

			return ss;
		}
	};

	struct FieldHeader
	{
		AssetMetadata::FieldData Data;

		friend std::ostream& operator <<(std::ostream& ss, const FieldHeader& header)
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

		friend std::istream& operator >>(std::istream& ss, FieldHeader& header)
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
	};

	void AssetMetadata::Clear()
	{
		for (auto& pair : FieldTable)
		{
			FieldData& data = pair.second;

			if (data.DataPtr)
			{
				if (data.IsMap)
					delete (AssetMetadata*)(data.DataPtr);
				else
					delete[] (uint8_t*)(data.DataPtr);

				data.DataPtr = nullptr;
			}
		}

		FieldTable.clear();
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
			FieldData& data = pair.second;

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

				uint32_t DataSize = FieldHeaderList[i].Data.DataSize;
				bool IsMap = FieldHeaderList[i].Data.IsMap;
				void* DataPtr = FieldHeaderList[i].Data.DataPtr;

				FieldTable[FieldName] = { DataSize, IsMap, DataPtr };
			}
		}
	}

	void* AssetMetadata::GetField(const std::string& field, size_t size)
	{
		void* DataPtr = FieldTable[field].DataPtr;
		size = FieldTable[field].DataSize;

		return DataPtr;
	}

	void AssetMetadata::Copy(AssetMetadata& CopyData)
	{
		FieldMap& CopyTable = CopyData.FieldTable;

		for (auto& [key, data] : FieldTable)
		{
			CopyTable[key].DataSize = data.DataSize;
			CopyTable[key].IsMap = data.IsMap;

			if (data.DataPtr)
			{
				if (data.IsMap)
				{
					AssetMetadata* copyMetadata = new AssetMetadata();
					((AssetMetadata*)data.DataPtr)->Copy(*copyMetadata);

					CopyTable[key].DataPtr = copyMetadata;
				}
				else
				{
					if (data.DataSize > 0)
					{
						uint8_t* buff = new uint8_t[data.DataSize];
						memcpy(buff, data.DataPtr, data.DataSize);
						
						CopyTable[key].DataPtr = buff;
					}
				}
			}
		}
	}
}