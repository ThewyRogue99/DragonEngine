#include "depch.h"
#include "Asset.h"

namespace Engine
{
	void Asset::Write(std::ostream& ss)
	{
		ss.write((char*)&Type, sizeof(Type));

		uint32_t NameLength = (uint32_t)(Name.size() + 1);
		ss.write((char*)&NameLength, sizeof(NameLength));

		ss.write((char*)Name.c_str(), NameLength * sizeof(wchar_t));

		Metadata->Write(ss);
	}

	void Asset::Read(std::istream& ss)
	{
		ss.read((char*)&Type, sizeof(Type));

		uint32_t NameLength = 0;
		ss.read((char*)&NameLength, sizeof(NameLength));

		wchar_t* str = new wchar_t[NameLength];
		ss.read((char*)str, NameLength * sizeof(wchar_t));

		Name = str;

		delete[] str;

		Metadata->Read(ss);
	}
}
