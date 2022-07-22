#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Events/Event.h"
#include "EditorCamera.h"

namespace Engine
{
	class EditorScene : public Scene
	{
	public:
		EditorScene(const CString& Name = TEXT("Editor Scene"));
		EditorScene(const EditorScene&) = default;

		~EditorScene();

		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnEvent(Event& event) override;

		virtual void OnViewportResize(uint32_t width, uint32_t height) override;

		virtual void OnSceneBegin() override;
		virtual void OnSceneEnd() override;

		Ref<EditorScene> CopyEditorScene();

		void BeginSimulation() { bShouldSimulate = true; }
		void EndSimulation() { bShouldSimulate = false; }

		inline EditorCamera* GetEditorCamera() const { return editorCamera; }

		inline void SetShouldBlockEvents(bool state) { bShouldBlockEvents = state; }

	private:
		EditorCamera* editorCamera = nullptr;

		bool bShouldSimulate = false;
		bool bShouldBlockEvents = false;
	};
}
