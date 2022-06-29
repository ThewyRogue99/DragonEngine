#pragma once

// These types are temporary
#include <string>

namespace Engine
{
	using CString = std::wstring;

	namespace TypeUtils
	{
		CString FromUTF8(const std::string& str);

		std::string FromUTF16(const CString& str);
	}
}