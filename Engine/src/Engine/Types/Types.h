#pragma once

#include "Engine/Core/Core.h"

// These types are temporary
#include <string>

#include <ostream>
#include <istream>

#include <unordered_map>

#include <fmt/format.h>

namespace Engine
{
	template<typename T>
	using StringBase = std::basic_string<T, std::char_traits<T>, std::allocator<T>>;

	using WString = StringBase<wchar_t>;
	using CString = StringBase<char>;

	namespace TypeUtils
	{
		ENGINE_API WString FromUTF8(const CString& str);

		ENGINE_API CString FromUTF16(const WString& str);

		template<typename... Args>
		CString FormatUTF8(const char* fmt, Args&&... args)
		{
			return fmt::format(fmt, std::forward<Args>(args)...);
		}
	}

	class MemoryMap
	{
	public:
		MemoryMap() = default;
		MemoryMap(const MemoryMap& Map)
		{
			Map.Copy(*this);
		}

		ENGINE_API ~MemoryMap();

		template<typename T>
		void SetField(const CString& field, T& value) { SetField(field, (void*)&value, sizeof(T)); }

		template<>
		void SetField(const CString& field, MemoryMap& value) { SetMapField(field, value); }

		template<typename T>
		void SetStringField(const CString& field, const StringBase<T>& value)
		{
			SetField(field, (void*)value.c_str(), (value.size() + 1) * sizeof(T));
		}

		ENGINE_API void SetField(const CString& field, void* buff, size_t buffsize);

		struct FieldData
		{
			uint32_t DataSize = 0;
			bool IsMap = false;

			void* DataPtr = nullptr;
		};

		template<typename T>
		T& GetField(const CString& field) const
		{
			const FieldData& Data = GetFieldData(field);

			return *((T*)Data.DataPtr);
		}

		template<typename T>
		StringBase<T> GetStringField(const CString& field) const
		{
			const FieldData& Data = GetFieldData(field);

			StringBase<T> s((const T*)Data.DataPtr);

			return s;
		}

		ENGINE_API void Write(std::ostream& ss);
		ENGINE_API void Read(std::istream& ss);

		ENGINE_API FieldData& GetFieldData(const CString& field);
		ENGINE_API const FieldData& GetFieldData(const CString& field) const;

		ENGINE_API void Copy(MemoryMap& CopyData) const;

		bool Empty() { return FieldTable.size() == 0; }

		using FieldMap = std::unordered_map<CString, FieldData>;

		FieldMap::iterator begin() { return FieldTable.begin(); }
		FieldMap::iterator end() { return FieldTable.end(); }

		FieldMap::const_iterator begin() const { return FieldTable.begin(); }
		FieldMap::const_iterator end() const { return FieldTable.end(); }

		size_t size() const { return FieldTable.size(); }

		bool FieldExists(const CString& field) { return FieldTable.find(field) != FieldTable.end(); }

		ENGINE_API void Clear();

		MemoryMap& operator=(MemoryMap& Other)
		{
			Other.Copy(*this);

			return *this;
		}

	private:
		ENGINE_API void SetMapField(const CString& field, MemoryMap& value);

	private:
		class TableHeader
		{
		public:
			TableHeader() = default;
			TableHeader(const TableHeader&) = default;

			CString fieldName;

			friend std::ostream& operator <<(std::ostream& ss, const TableHeader& header)
			{
				return TableHeader::WriteStream(ss, header);
			}

			friend std::istream& operator >>(std::istream& ss, TableHeader& header)
			{
				return TableHeader::ReadStream(ss, header);
			}

		private:
			ENGINE_API static std::ostream& WriteStream(std::ostream& ss, const TableHeader& header);
			ENGINE_API static std::istream& ReadStream(std::istream& ss, TableHeader& header);
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
			ENGINE_API static std::ostream& WriteStream(std::ostream& ss, const FieldHeader& header);
			ENGINE_API static std::istream& ReadStream(std::istream& ss, FieldHeader& header);
		};

	private:
		FieldMap FieldTable = { };
	};
}