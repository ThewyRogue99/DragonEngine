#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"

#include "Engine/Core/UUID.h"

#include "entt.hpp"

namespace Engine
{
	class Scene;

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

			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = CurrentScene->SceneRegistry.emplace_or_replace<T>(EntityHandle, std::forward<Args>(args)...);

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

		UUID GetUUID();

		friend class Scene;

	private:
		entt::entity EntityHandle = entt::null;

		Scene* CurrentScene = nullptr;
	};
}