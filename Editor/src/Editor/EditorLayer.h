#pragma once

#include "Engine/Core/Core.h"

#include "Project/ProjectManager.h"
#include "Engine/Core/Layer.h"
#include "Panels/PanelManager.h"
#include "Scene/EditorScene.h"

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float DeltaTime) override;

		void OnImGuiRender(float DeltaTime);

		virtual void OnEvent(Event& event) override;

		void OpenSettingsPanel();

	private:
		void OnActiveSceneChange(Scene* scene);

		void OnBeginPlay();
		void OnEndPlay();

		void OnProjectLoad(const Project& project);

	private:
		struct LayerData
		{
			bool bIsSettingsOpen = false;
		} Data;

	private:
		Scene* ActiveScene = nullptr;

		PanelManager PManager;
	};
}
