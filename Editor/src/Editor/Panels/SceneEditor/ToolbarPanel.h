#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Editor/Panels/EditorPanel.h"
#include "Editor/Scene/EditorScene.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class ToolbarPanel : public EditorPanel
	{
	public:
		ToolbarPanel();

		virtual void OnCreate() override;

		virtual void OnRender(float DeltaTime) override;

	private:
		void OnEditorSceneChange(EditorScene* NewScene);

		void OnBeginPlay();
		void OnEndPlay();

	private:
		enum SceneState
		{
			Play, Edit, Simulate
		};

		SceneState ActiveSceneState = SceneState::Edit;

		Ref<Texture2D> PlayIcon = nullptr, StopIcon = nullptr, SimulateIcon = nullptr;

		Scene* ActiveScene = nullptr;
	};
}
