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
		void OnSetActiveScene(Ref<Scene> NewScene);

	private:
		Ref<Texture2D> PlayIcon = nullptr, StopIcon = nullptr;

		Ref<EditorScene> ActiveScene = nullptr;
	};
}
