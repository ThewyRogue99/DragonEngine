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

	protected:
		virtual void OnData(const CString& Name, void* Data, size_t size) override;

	private:
		Ref<Texture2D> PlayIcon, StopIcon;

		EditorScene* ActiveScene;
	};
}
