#pragma once

#include "Engine/Types/Types.h"

namespace Engine
{
	class FileDialogs
	{
	public:
		static CString OpenFile(const wchar_t* filter);
		static CString SaveFile(const wchar_t* filter);
	};
}