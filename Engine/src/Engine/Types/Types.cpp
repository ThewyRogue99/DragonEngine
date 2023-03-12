#include "depch.h"
#include "Types.h"

#include <codecvt>
#include <cstdarg>

namespace Engine
{
	namespace TypeUtils
	{
#pragma warning(push, 0)
		CString FromUTF8(const std::string& str)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
		}

		std::string FromUTF16(const CString& str)
		{
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
		}
#pragma warning(pop, 0)
	}

	MemoryMap::~MemoryMap()
	{
		Clear();
	}

	void MemoryMap::SetField(const std::string& field, void* buff, size_t buffsize)
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

	void MemoryMap::SetField(const std::string& field, MemoryMap& data)
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

	MemoryMap::FieldData& MemoryMap::GetFieldData(const std::string& field)
	{
		return FieldTable[field];
	}

	const MemoryMap::FieldData& MemoryMap::GetFieldData(const std::string& field) const
	{
		return FieldTable.at(field);
	}

	void MemoryMap::Copy(MemoryMap& CopyData)
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
}