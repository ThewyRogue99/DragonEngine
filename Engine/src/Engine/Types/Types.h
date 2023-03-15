#pragma once

// These types are temporary
#include <string>

#include <ostream>
#include <istream>

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/format.h>
#pragma warning(pop)

namespace Engine
{
	template<typename T>
	using StringBase = std::basic_string<T, std::char_traits<T>, std::allocator<T>>;

	using CString = StringBase<wchar_t>;

	namespace TypeUtils
	{
		CString FromUTF8(const std::string& str);

		std::string FromUTF16(const CString& str);

		template<typename... Args>
		std::string FormatUTF8(const char* fmt, Args&&... args)
		{
			return fmt::vformat(fmt, fmt::make_format_args(std::forward<Args>(args)...));
		}
	}

	class MemoryMap
	{
	public:
		MemoryMap() = default;
		MemoryMap(MemoryMap& Map)
		{
			Map.Copy(*this);
		}

		~MemoryMap();

		template<typename T>
		void SetField(const std::string& field, T& value) { SetField(field, (void*)&value, sizeof(T)); }

		template<typename T>
		void SetStringField(const std::string& field, const StringBase<T>& value)
		{
			SetField(field, (void*)value.c_str(), (value.size() + 1) * sizeof(T));
		}

		void SetField(const std::string& field, void* buff, size_t buffsize);

		void SetField(const std::string& field, MemoryMap& data);

		struct FieldData
		{
			uint32_t DataSize = 0;
			bool IsMap = false;

			void* DataPtr = nullptr;
		};

		template<typename T>
		T& GetField(const std::string& field) const
		{
			const FieldData& Data = GetFieldData(field);

			return *((T*)Data.DataPtr);
		}

		template<typename T>
		StringBase<T> GetStringField(const std::string& field) const
		{
			const FieldData& Data = GetFieldData(field);

			StringBase<T> s((const T*)Data.DataPtr);

			return s;
		}

		void Write(std::ostream& ss);
		void Read(std::istream& ss);

		FieldData& GetFieldData(const std::string& field);
		const FieldData& GetFieldData(const std::string& field) const;

		void Copy(MemoryMap& CopyData);

		bool Empty() { return FieldTable.size() == 0; }

		using FieldMap = std::unordered_map<std::string, FieldData>;

		FieldMap::iterator begin() { return FieldTable.begin(); }
		FieldMap::iterator end() { return FieldTable.end(); }

		FieldMap::const_iterator begin() const { return FieldTable.begin(); }
		FieldMap::const_iterator end() const { return FieldTable.end(); }

		size_t size() const { return FieldTable.size(); }

		bool FieldExists(const std::string& field) { return FieldTable.find(field) != FieldTable.end(); }

		void Clear();

		MemoryMap& operator=(MemoryMap& Other)
		{
			Other.Copy(*this);

			return *this;
		}

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

	private:
		FieldMap FieldTable = { };
	};
}