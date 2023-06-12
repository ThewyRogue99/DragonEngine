#include "depch.h"
#include "Types.h"

#include <codecvt>

namespace Engine
{
	namespace TypeUtils
	{
#pragma warning(push, 0)
		WString FromUTF8(const CString& str)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
		}

		CString FromUTF16(const WString& str)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
		}
#pragma warning(pop, 0)
	}

	MemoryMap::~MemoryMap()
	{
		Clear();
	}

	void MemoryMap::SetField(const CString& field, void* buff, size_t buffsize)
	{
		uint8_t* databuff = new uint8_t[buffsize];

		memcpy(databuff, buff, buffsize);

		auto it = FieldTable.find(field);
		if (it != FieldTable.end())
		{
			FieldData& data = it->second;

			if (data.DataPtr)
			{
				if (data.IsMap)
					delete (MemoryMap*)data.DataPtr;
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

	void MemoryMap::SetMapField(const CString& field, MemoryMap& data)
	{
		MemoryMap* copyData = new MemoryMap();
		data.Copy(*copyData);

		auto it = FieldTable.find(field);
		if (it != FieldTable.end())
		{
			FieldData& data = it->second;

			if (data.DataPtr)
			{
				if (data.IsMap)
					delete (MemoryMap*)data.DataPtr;
				else
					delete[] data.DataPtr;
			}

			data.DataPtr = copyData;
			data.DataSize = 0;
		}
		else
		{
			FieldTable[field] = { (uint32_t)(data.FieldTable.size()), true, copyData };
		}
	}

	MemoryMap::FieldData& MemoryMap::GetFieldData(const CString& field)
	{
		return FieldTable[field];
	}

	const MemoryMap::FieldData& MemoryMap::GetFieldData(const CString& field) const
	{
		return FieldTable.at(field);
	}

	void MemoryMap::Copy(MemoryMap& CopyData) const
	{
		FieldMap& CopyTable = CopyData.FieldTable;
		CopyTable.clear();

		for (auto& [key, data] : FieldTable)
		{
			CopyTable[key].DataSize = data.DataSize;
			CopyTable[key].IsMap = data.IsMap;

			if (data.DataPtr)
			{
				if (data.IsMap)
				{
					MemoryMap* copyMap = new MemoryMap();
					((MemoryMap*)data.DataPtr)->Copy(*copyMap);

					CopyTable[key].DataPtr = copyMap;
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

	void MemoryMap::Clear()
	{
		for (auto& pair : FieldTable)
		{
			FieldData& data = pair.second;

			if (data.DataPtr)
			{
				if (data.IsMap)
					delete (MemoryMap*)data.DataPtr;
				else
					delete[] (uint8_t*)data.DataPtr;

				data.DataPtr = nullptr;
			}
		}

		FieldTable.clear();
	}

	void MemoryMap::Write(std::ostream& ss)
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
			h.Data = data;

			ss << h;
		}
	}

	void MemoryMap::Read(std::istream& ss)
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
				CString FieldName = TableHeaderList[i].fieldName;

				MemoryMap::FieldData& data = FieldTable[FieldName];

				data.DataSize = FieldHeaderList[i].Data.DataSize;
				data.IsMap = FieldHeaderList[i].Data.IsMap;
				data.DataPtr = FieldHeaderList[i].Data.DataPtr;
			}
		}
	}

	std::ostream& MemoryMap::TableHeader::WriteStream(std::ostream& ss, const TableHeader& header)
	{
		uint32_t nameSize = (uint32_t)(header.fieldName.size() + 1);

		ss.write((char*)&nameSize, sizeof(uint32_t));

		ss.write(header.fieldName.c_str(), nameSize);

		return ss;
	}

	std::istream& MemoryMap::TableHeader::ReadStream(std::istream& ss, TableHeader& header)
	{
		uint32_t nameSize = 0;
		ss.read((char*)&nameSize, sizeof(uint32_t));

		char* buff = new char[nameSize];
		ss.read(buff, nameSize);

		header.fieldName = buff;
		delete[] buff;

		return ss;
	}

	std::ostream& MemoryMap::FieldHeader::WriteStream(std::ostream& ss, const FieldHeader& header)
	{
		if (header.Data.DataPtr)
		{
			ss.write((char*)&header.Data.DataSize, sizeof(uint32_t));
			ss.write((char*)&header.Data.IsMap, sizeof(bool));

			if (header.Data.IsMap)
			{
				MemoryMap* memMap = (MemoryMap*)header.Data.DataPtr;
				memMap->Write(ss);
			}
			else
			{
				ss.write((char*)header.Data.DataPtr, header.Data.DataSize);
			}
		}

		return ss;
	}

	std::istream& MemoryMap::FieldHeader::ReadStream(std::istream& ss, FieldHeader& header)
	{
		uint32_t DataSize = 0;
		ss.read((char*)&DataSize, sizeof(uint32_t));

		bool IsMap = false;
		ss.read((char*)&IsMap, sizeof(bool));

		if (IsMap)
		{
			MemoryMap* memMap = new MemoryMap();
			memMap->Read(ss);

			header.Data.DataSize = DataSize;
			header.Data.IsMap = true;
			header.Data.DataPtr = memMap;
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