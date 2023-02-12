#pragma once

// These types are temporary
#include <string>

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
}