#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "EditorPanel.h"
#include "../Scene/EditorScene.h"
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
		void OnSetActiveScene(Scene* NewScene);

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
