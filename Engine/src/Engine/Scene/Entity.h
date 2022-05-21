#pragma once

#include "Scene.h"

#include "entt.hpp"

namespace Engine
{
	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);

		template<typename T, typename... Args>
		inline T& AddComponent(Args&&... args)
		{
			DE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			return CurrentScene->SceneRegistry.emplace<T>(EntityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		inline T& GetComponent()
		{
			DE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");

			return CurrentScene->SceneRegistry.get<T>(EntityHandle);
		}

		template<typename T>
		inline bool HasComponent()
		{
			return CurrentScene->SceneRegistry.all_of<T>(EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			DE_CORE_ASSERT(HasComponent<T>(), "Entity doesn't have a component!");

			CurrentScene->SceneRegistry.remove<T>(EntityHandle);
		}

		inline bool IsValid() { return EntityHandle != entt::null; }

	private:
		entt::entity EntityHandle = entt::null;
		Scene* CurrentScene = nullptr;
	};
}