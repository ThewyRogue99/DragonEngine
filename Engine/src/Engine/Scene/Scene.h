#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"
#include "Engine/Renderer/Camera.h"

#include "Entity.h"

class b2World;

namespace Engine
{
	class Entity;
	class UUID;
	struct CameraComponent;

	class Scene
	{
	public:
		Scene(const CString& Name = TEXT("Scene"));
		Scene(const Scene&) = default;

		Entity CreateEntity(const CString& name = CString());
		Entity CreateEntityWithUUID(UUID uuid, const CString& name = CString());

		Entity GetEntityWithUUID(UUID uuid);

		void DestroyEntity(Entity entity);

		void OnSceneStart();
		void OnSceneStop();

		virtual void OnUpdate(float DeltaTime);

		virtual void OnViewportResize(uint32_t width, uint32_t height);

		enum class SceneState
		{
			Edit = 0, Play, Stop
		};

		void SetSceneState(SceneState state);
		inline const SceneState GetSceneState() const { return CurrentSceneState; }

		virtual void OnEvent(Event& event);

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;

		inline const CString& GetName() const { return SceneName; }
		inline void SetName(const CString& Name) { SceneName = Name; }

		Ref<Scene> Copy();

		struct CameraProps
		{
			Camera* CameraPtr = nullptr;
			Entity EntityHandle = Entity();
		};

		inline CameraProps& GetPrimaryCameraProps() { return PrimaryCamera; }

	private:
		void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

		void CopyEntity(Ref<Scene> TargetScene, entt::entity entity);

		template<typename Component>
		void CopyComponent(Ref<Scene> TargetScene, entt::entity TargetEntity, entt::entity SourceEntity);

	protected:
		CString SceneName;

		entt::registry SceneRegistry;

		b2World* m_PhysicsWorld = nullptr;

		CameraProps PrimaryCamera;

		SceneState CurrentSceneState = SceneState::Stop;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
