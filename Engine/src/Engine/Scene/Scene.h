#pragma once

#include "Entity.h"

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"
#include "Engine/Renderer/Camera.h"

#include "Engine/Physics/PhysicsWorld2D.h"

#include "Components.h"

#include <vector>

class b2World;

namespace Engine
{
	class Scene
	{
	public:
		ENGINE_API Entity CreateEntity(const WString& name = WString());
		ENGINE_API Entity CreateEntityWithUUID(UUID uuid, const WString& name = WString());
		ENGINE_API Entity CopyEntity(Entity entity);

		ENGINE_API Entity GetEntityWithUUID(UUID uuid);

		ENGINE_API void DestroyEntity(Entity entity);

		ENGINE_API virtual void OnUpdate(float DeltaTime);

		ENGINE_API virtual void OnViewportResize(uint32_t width, uint32_t height);

		ENGINE_API virtual void OnEvent(Event& event);

		size_t GetEntityCount() const { return SceneRegistry.size(); }
		ENGINE_API std::vector<Entity> GetEntities();

		inline const WString& GetName() const { return SceneName; }
		inline void SetName(const WString& Name) { SceneName = Name; }

		ENGINE_API void Copy(Scene* SceneRef);

		inline virtual Camera* GetPrimaryCamera() { return PrimaryCamera; }

		ENGINE_API virtual void OnSceneBegin();
		ENGINE_API virtual void OnSceneEnd();

		friend class Entity;
		friend class SceneManager;
		friend class SceneSerializer;
		friend class PhysicsWorld2D;
		friend class SceneHierarchyPanel;
		friend class SceneRenderer;

	protected:
		ENGINE_API Scene(const WString& Name = TEXT("Scene"));
		Scene(const Scene&) = default;

		~Scene() = default;

	private:
		ENGINE_API void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

		ENGINE_API void CopyEntity(Scene* TargetScene, Entity entity);

		template<typename... Component>
		ENGINE_API void CopyComponent(Entity TargetEntity, Entity SourceEntity);

		template<typename... Component>
		ENGINE_API void CopyComponent(ComponentGroup<Component...>, Entity TargetEntity, Entity SourceEntity);

	protected:
		WString SceneName;

		entt::registry SceneRegistry;

		PhysicsWorld2D PhysicsWorld;

		Camera* PrimaryCamera = nullptr;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
