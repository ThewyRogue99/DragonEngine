#include "ProjectTools.h"

#include "Engine/Scripting/ScriptEngine.h"

#include <sstream>
#include <cstdlib>

#include <filesystem>

namespace Engine
{
	namespace ProjectTools
	{
#ifdef ENGINE_PLATFORM_WINDOWS
	#ifdef ENGINE_BUILD_DEBUG
		#define PREMAKE_PATH "..\\vendor\\premake\\bin\\premake5.exe"
		#define SCRIPT_CORE_PATH L"..\\bin\\Resources\\Scripts\\DE_ScriptCore.dll"
	#else
		#define PREMAKE_PATH "vendor\\premake\\premake5.exe"
		#define SCRIPT_CORE_PATH L"Resources\\Scripts\\DE_ScriptCore.dll"
	#endif

#define CORE_ASSEMBLY_RELATIVE_PATH TEXT("Binary\\Scripts\\DE_ScriptCore.dll")
#define APP_ASSEMBLY_RELATIVE_PATH TEXT("Binary\\Scripts\\ScriptAssembly.dll")

		static std::filesystem::path ProjectPath;

		static std::filesystem::path CoreAssemblyPath;
		static std::filesystem::path AppAssemblyPath;

		static std::filesystem::path GetSolutionPath()
		{
			return ProjectPath / TEXT("GameAssembly.sln");
		}

		static bool CoreAssemblyExists()
		{
			return std::filesystem::exists(CoreAssemblyPath);
		}

		static bool CopyCoreAssembly()
		{
			return std::filesystem::copy_file(SCRIPT_CORE_PATH, CoreAssemblyPath, std::filesystem::copy_options::overwrite_existing);
		}

		static bool RemoveScriptAssembly()
		{
			std::error_code ec;
			return std::filesystem::remove(ProjectPath / APP_ASSEMBLY_RELATIVE_PATH, ec);
		}

		void GenerateScriptProject()
		{
			std::filesystem::path LuaPath = ProjectPath / TEXT("ProjGen.lua");

			std::filesystem::copy_file(TEXT("Resource\\Scripts\\ProjGen.lua"), LuaPath);

			std::string premakeFilePath = TypeUtils::FromUTF16(LuaPath);
			std::stringstream ss;
			ss << "Resource\\Scripts\\Win-SolutionGen.bat " << PREMAKE_PATH << ' ' << premakeFilePath;

			system(ss.str().c_str());

			std::filesystem::remove(LuaPath);

			if(!CoreAssemblyExists())
				CopyCoreAssembly();
		}

		bool CompileScriptProject()
		{
			if (!CoreAssemblyExists() && !CopyCoreAssembly())
				return false;

			std::filesystem::path SolutionPath = GetSolutionPath();

			if (ScriptProjectExists())
			{
				RemoveScriptAssembly();

				std::stringstream ss;
				ss << "C:\\Windows\\Microsoft.NET\\Framework64\\v4.0.30319\\MSBuild.exe ";
				ss << TypeUtils::FromUTF16(SolutionPath) << " /p:DebugType=pdbonly";

				system(ss.str().c_str());

				LoadScriptProject();

				return true;
			}

			return false;
		}
#endif
		bool LoadScriptProject()
		{
			ScriptEngine::SetAssemblyPath(
				CoreAssemblyPath,
				AppAssemblyPath
			);

			return ScriptEngine::Load();
		}

		bool ScriptProjectExists()
		{
			std::filesystem::path SolutionPath = GetSolutionPath();

			return std::filesystem::exists(SolutionPath);
		}

		bool ScriptAssemblyExists()
		{
			return std::filesystem::exists(CoreAssemblyPath) &&
				std::filesystem::exists(AppAssemblyPath);
		}

		void SetProjectPath(const CString& NewPath)
		{
			ProjectPath = NewPath;

			CoreAssemblyPath = ProjectPath / CORE_ASSEMBLY_RELATIVE_PATH;
			AppAssemblyPath = ProjectPath / APP_ASSEMBLY_RELATIVE_PATH;
		}

		std::string GetScriptTemplateString(const std::string& ScriptName)
		{
			std::stringstream ss;

			ss <<
R"(using DragonEngine;

public class )" << ScriptName << R"( : Script
{
	public void BeginPlay()
	{

	}

	public void Update(float DeltaTime)
	{

	}
})";		return ss.str();
		}
	}
}