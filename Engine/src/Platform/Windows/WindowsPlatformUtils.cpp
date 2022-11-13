#include "depch.h"

#ifdef ENGINE_PLATFORM_WINDOWS

#include <Windows.h>

#include "Engine/Utils/PlatformUtils.h"
#include "Engine/Core/Application.h"

#include <commdlg.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Engine
{
	CString FileDialogs::OpenFile(const wchar_t* filter)
	{
		OPENFILENAMEW ofn;
		WCHAR szFile[MAX_PATH] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameW(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		return CString();
	}

	CString FileDialogs::SaveFile(const wchar_t* filter)
	{
		OPENFILENAMEW ofn;
		WCHAR szFile[MAX_PATH] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetSaveFileNameW(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		return CString();
	}
}

#endif