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
		CString Name;
		WString Path;

		CString StartSceneID;

		friend class ProjectManager;

	private:
		void Serialize(Ref<AssetMetadata> data);
		void Deserialize(Ref<AssetMetadata> data);
	};

	class ProjectManager
	{
	public:
		static bool CreateProject(const WString& ProjectPath, const CString& ProjectName);

		static bool LoadProject(const WString& ProjectPath);

		static bool CreateScript(const CString& Name);

		static bool IsProjectLoaded();

		static const Project& GetLoadedProject();

		static CallbackDispatcher<void, const Project&>::CallbackHandle OnLoadProject();
	};
}