#pragma once

// These types are temporary
#include <string>

namespace Engine
{
	template<typename T>
	using StringBase = std::basic_string<T, std::char_traits<T>, std::allocator<T>>;

	using CString = StringBase<wchar_t>;

	namespace TypeUtils
	{
		CString FromUTF8(const std::string& str);

		std::string FromUTF16(const CString& str);
	}
}