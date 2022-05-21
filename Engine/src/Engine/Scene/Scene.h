#pragma once

#include "entt.hpp"

namespace Engine
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& name = std::string());

		void OnUpdate(Timestep timestep);

		friend class Entity;

	private:
		entt::registry SceneRegistry;
	};
}
