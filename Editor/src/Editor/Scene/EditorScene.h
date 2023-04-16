#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Events/Event.h"
#include "EditorCamera.h"

namespace Engine
{
	class EditorScene : public Scene
	{
	public:
		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnEvent(Event& event) override;

		virtual void OnViewportResize(uint32_t width, uint32_t height) override;

		virtual void OnSceneBegin() override;
		virtual void OnSceneEnd() override;

		void CopyEditorScene(EditorScene* SceneRef);

		void BeginSimulation() { bShouldSimulate = true; }
		void EndSimulation() { bShouldSimulate = false; }

		inline void SetShouldBlockEvents(bool state) { bShouldBlockEvents = state; }

		friend class EditorSceneManager;
		friend class ContentBrowserPanel;

	private:
		EditorScene(const WString& Name = TEXT("Editor Scene"));
		EditorScene(const EditorScene&) = default;

		~EditorScene() = default;

	private:
		bool bShouldSimulate = false;
		bool bShouldBlockEvents = false;
	};
}
