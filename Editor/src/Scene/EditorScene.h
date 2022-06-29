#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Events/Event.h"
#include "Engine/Renderer/EditorCamera.h"

namespace Engine
{
	class EditorScene : public Scene
	{
	public:
		EditorScene();
		EditorScene(const EditorScene&) = default;

		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnEvent(Event& event) override;

		virtual void OnViewportResize(uint32_t width, uint32_t height) override;

		inline const EditorCamera& GetEditorCamera() const { return editorCamera; }

	private:
		EditorCamera editorCamera;
	};
}
