#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Layer.h"
#include "Panels/PanelManager.h"
#include "Scene/EditorScene.h"
#include "Engine/Scene/SceneManager.h"

#include <imgui/imgui.h>

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

		void OpenScene();
		void OpenScene(const CString& path);

		void SaveScene();
		void SaveScene(const CString& path);

		void OnActiveSceneChange(Scene* scene);

	private:
		EditorScene* ActiveScene;

		// Panels
		PanelManager PManager;
	};
}
