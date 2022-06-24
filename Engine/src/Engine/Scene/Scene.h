#pragma once

#include "entt.hpp"
#include "Engine/Core/Timestep.h"
#include "Engine/Renderer/EditorCamera.h"

class b2World;

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

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateEditor(Timestep timestep, EditorCamera& camera);
		void OnUpdateRuntime(Timestep timestep);

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

		b2World* m_PhysicsWorld = nullptr;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
