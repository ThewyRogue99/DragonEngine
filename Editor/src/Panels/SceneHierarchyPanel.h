#pragma once

#include "Engine/Core/Core.h"
#include "EditorPanel.h"
#include "../Scene/EditorScene.h"
#include "Engine/Scene/Entity.h"

namespace Engine
{
	class SceneHierarchyPanel : public EditorPanel
	{
	public:
		SceneHierarchyPanel();

		virtual void OnCreate() override;

		virtual void OnRender(float DeltaTime) override;

		Entity GetSelectedEntity() const { return SelectedEntity; }
		void SetSelectedEntity(Entity entity, bool SetData = true);

	protected:
		virtual void OnData(const CString& Name, void* Data, size_t size) override;

	private:
		void DrawEntityNode(Entity entity);

		void DrawComponents(Entity entity);

		void OnSetActiveScene(Scene* NewScene);

	private:
		Scene* Context = nullptr;

		Entity SelectedEntity = { };
	};
}
