#pragma once

#include "Entity.h"

#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"
#include "Engine/Renderer/Camera.h"

#include "Components.h"

class b2World;

namespace Engine
{
	class Scene
	{
	public:
		Scene(const CString& Name = TEXT("Scene"));
		Scene(const Scene&) = default;

		Entity CreateEntity(const CString& name = CString());
		Entity CreateEntityWithUUID(UUID uuid, const CString& name = CString());

		Entity GetEntityWithUUID(UUID uuid);

		void DestroyEntity(Entity entity);

		virtual void OnUpdate(float DeltaTime);

		virtual void OnViewportResize(uint32_t width, uint32_t height);

		virtual void OnEvent(Event& event);

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;

		inline const CString& GetName() const { return SceneName; }
		inline void SetName(const CString& Name) { SceneName = Name; }

		Scene* Copy();

		inline virtual Camera* GetPrimaryCamera() { return PrimaryCamera; }

		virtual void OnSceneBegin();
		virtual void OnSceneEnd();

	protected:
		void Render(float DeltaTime);

		void OnPhysics2DStart();
		void OnPhysics2DUpdate(float DeltaTime);
		void OnPhysics2DEnd();

		void CopyToRef(Scene* SceneRef);

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

		b2World* m_PhysicsWorld = nullptr;

		Camera* PrimaryCamera = nullptr;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
