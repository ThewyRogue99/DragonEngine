#pragma once

#include "Engine/Core/Core.h"

#include "Project/ProjectManager.h"
#include "Engine/Core/Layer.h"
#include "Panels/PanelManager.h"
#include "Scene/EditorScene.h"
#include "Engine/Scene/SceneManager.h"

namespace Engine
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnImGuiRender(float DeltaTime) override;

		virtual void OnEvent(Event& event) override;

	private:
		void NewScene();

		void OnActiveSceneChange(Scene* scene);

		void OnProjectLoad(const Project& project);

	private:
		EditorScene* ActiveScene = nullptr;

		// Panels
		PanelManager PManager;
	};
}
