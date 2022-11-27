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

		// For ProjectManager
		void SetProjectPath(const CString& NewPath);

		std::string GetScriptTemplateString(const std::string& ScriptName);
	}
}