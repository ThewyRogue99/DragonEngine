#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Core/CallbackDispatcher.h"

namespace Engine
{
	class AssetMetadata;

	class Project
	{
	public:
		std::string Name;
		CString Path;

		std::string StartSceneID;

		friend class ProjectManager;

	private:
		void Serialize(AssetMetadata& data);
		void Deserialize(AssetMetadata& data);
	};

	class ProjectManager
	{
	public:
		static bool CreateProject(const CString& ProjectPath, const std::string& ProjectName);

		static bool LoadProject(const CString& ProjectPath);

		static bool CreateScript(const std::string& Name);

		static bool IsProjectLoaded();

		static const Project& GetLoadedProject();

		static CallbackDispatcher<const Project&>::CallbackHandle OnLoadProject();
	};
}