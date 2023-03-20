#pragma once

#include "EditorPanel.h"
#include "PanelManager.h"

#include "Engine/Events/Event.h"

namespace Engine
{
	class Project;

	class SceneEditorPanel : public EditorPanel
	{
	public:
		SceneEditorPanel(const std::string& Name = "Scene Editor")
			: EditorPanel(Name) { }

		virtual void OnCreate() override;

		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnRender(float DeltaTime) override;

		virtual void OnEvent(Event& event) override;

	private:
		void OnProjectLoad(const Project&);

	private:
		struct PanelData
		{
			bool bConsoleOpen = true;
			bool bContentBrowserOpen = true;
			bool bSceneHierarchyOpen = true;
			bool bPerformanceOpen = false;
		} Data;

	private:
		PanelManager PManager;
	};
}