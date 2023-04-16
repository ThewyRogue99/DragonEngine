#pragma once

#include "ProjectManager.h"

namespace Engine
{
	namespace ProjectTools
	{
		void GenerateScriptProject();

		bool CompileScriptProject();

		bool ScriptProjectExists();
		bool ScriptAssemblyExists();

		bool LoadScriptProject();
		void OpenScriptProject();

		// ** For ProjectManager **
		void SetProjectPath(const WString& NewPath);

		CString GetScriptTemplateString(const CString& ScriptName);
	}
}