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
		Entity CreateEntity(const CString& name = CString());
		Entity CreateEntityWithUUID(UUID uuid, const CString& name = CString());
		Entity CopyEntity(Entity entity);

		Entity GetEntityWithUUID(UUID uuid);

		void DestroyEntity(Entity entity);

		virtual void OnUpdate(float DeltaTime);

		virtual void OnViewportResize(uint32_t width, uint32_t height);

		virtual void OnEvent(Event& event);

		size_t GetEntityCount() const { return SceneRegistry.size(); }
		std::vector<Entity> GetEntities();

		inline const CString& GetName() const { return SceneName; }
		inline void SetName(const CString& Name) { SceneName = Name; }

		void Copy(Scene* SceneRef);

		inline virtual Camera* GetPrimaryCamera() { return PrimaryCamera; }

		virtual void OnSceneBegin();
		virtual void OnSceneEnd();

		friend class Entity;
		friend class SceneManager;
		friend class SceneSerializer;
		friend class PhysicsWorld2D;
		friend class SceneHierarchyPanel;
		friend class SceneRenderer;

	protected:
		Scene(const CString& Name = TEXT("Scene"));
		Scene(const Scene&) = default;

		~Scene() = default;

	private:
		void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

		void CopyEntity(Scene* TargetScene, Entity entity);

		template<typename... Component>
		void CopyComponent(Entity TargetEntity, Entity SourceEntity);

		template<typename... Component>
		void CopyComponent(ComponentGroup<Component...>, Entity TargetEntity, Entity SourceEntity);

	protected:
		CString SceneName;

		entt::registry SceneRegistry;

		PhysicsWorld2D PhysicsWorld;

		Camera* PrimaryCamera = nullptr;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
