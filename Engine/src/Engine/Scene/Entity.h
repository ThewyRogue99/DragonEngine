#pragma once

#include "Scene.h"

#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"

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

			T& component = CurrentScene->SceneRegistry.emplace<T>(EntityHandle, std::forward<Args>(args)...);

			CurrentScene->OnComponentAdded<T>(*this, component);

			return component;
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

		bool IsValid();

		operator uint32_t() const { return (uint32_t)EntityHandle; }

		inline bool operator ==(const Entity& other) const
		{
			return EntityHandle == other.EntityHandle && CurrentScene == other.CurrentScene;
		}

		inline bool operator !=(const Entity& other) const
		{
			return !(*this == other);
		}

		friend class Scene;

	private:
		entt::entity EntityHandle = entt::null;

		Scene* CurrentScene = nullptr;
	};
}