#pragma once

#include "Engine/Scene/Scene.h"
#include "Engine/Core/Core.h"

#include "Engine/Scene/Components.h"

namespace Engine
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;

		SceneHierarchyPanel(const Ref<Scene>& context);

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

		Entity GetSelectedEntity() const { return SelectedEntity; }
		void SetSelectedEntity(Entity entity);

	private:
		void DrawEntityNode(Entity entity);

		void DrawComponents(Entity entity);

	private:
		Ref<Scene> Context;

		Entity SelectedEntity;
	};
}
