#include "depch.h"
#include "Types.h"

#include <codecvt>

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
}