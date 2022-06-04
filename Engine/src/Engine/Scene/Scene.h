#pragma once

#include "entt.hpp"
#include "Engine/Core/Timestep.h"

namespace Engine
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void DestroyEntity(Entity entity);

		void OnUpdate(Timestep timestep);

		void OnViewportResize(uint32_t width, uint32_t height);

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;

		Entity GetPrimaryCameraEntity();

	private:
		template<typename T>
		void OnComponentAdded(Entity entity, T& component);

	private:
		entt::registry SceneRegistry;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
