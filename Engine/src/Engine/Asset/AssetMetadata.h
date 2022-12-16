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
		AssetMetadata() = default;
		AssetMetadata(const AssetMetadata&) = default;

		~AssetMetadata();

		struct FieldData
		{
			uint32_t DataSize = 0;
			bool IsMap = false;

			void* DataPtr = nullptr;
		};

		using FieldMap = std::unordered_map<std::string, FieldData>;

		template<typename T>
		void SetField(const std::string& field, T& value) { SetField(field, (void*)&value, sizeof(T)); }

		template<typename T>
		void SetStringField(const std::string& field, const StringBase<T>& value)
		{
			SetField(field, (void*)value.c_str(), (value.size() + 1) * sizeof(T));
		}

		void SetField(const std::string& field, void* buff, size_t buffsize);

		void SetField(const std::string& field, AssetMetadata& data);

		template<typename T>
		T& GetField(const std::string& field) const
		{
			size_t size = 0;
			void* DataPtr = GetField(field, size);

			return *((T*)DataPtr);
		}

		template<typename T>
		StringBase<T> GetStringField(const std::string& field)
		{
			size_t size = 0;
			void* DataPtr = GetField(field, size);

			StringBase<T> s((const T*)DataPtr);

			return s;
		}

		void* GetField(const std::string& field, size_t& size) const;

		void Copy(AssetMetadata& CopyData);

		bool Empty() { return FieldTable.size() == 0; }

		FieldMap::iterator begin() { return FieldTable.begin(); }
		FieldMap::iterator end() { return FieldTable.end(); }

		FieldMap::const_iterator begin() const { return FieldTable.begin(); }
		FieldMap::const_iterator end() const { return FieldTable.end(); }

		bool FieldExists(const std::string& field) { return FieldTable.find(field) != FieldTable.end(); }

		void Clear();

		void Write(std::ostream& ss);
		void Read(std::istream& ss);

	private:
		FieldMap FieldTable = { };
	};
}