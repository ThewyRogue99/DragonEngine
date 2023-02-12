#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Core/Log.h"

#include "Engine/Debug/Debug.h"

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
		T& AddComponent(Args&&... args)
		{
			DE_ASSERT(!HasComponent<T>(), "Entity already has component!");

			T& component = GetSceneRegistry().emplace<T>(EntityHandle, std::forward<Args>(args)...);

			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = GetSceneRegistry().emplace_or_replace<T>(EntityHandle, std::forward<Args>(args)...);

			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			DE_ASSERT(HasComponent<T>(), "Entity doesn't have component!");

			return GetSceneRegistry().get<T>(EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return GetSceneRegistry().all_of<T>(EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			DE_ASSERT(HasComponent<T>(), "Entity doesn't have a component!");

			GetSceneRegistry().remove<T>(EntityHandle);
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

		UUID& GetUUID();

		friend class Scene;

	private:
		entt::registry& GetSceneRegistry();

	private:
		entt::entity EntityHandle = entt::null;

		Scene* CurrentScene = nullptr;
	};
}