#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Events/Event.h"

#include "entt.hpp"

class b2World;

namespace Engine
{
	class Entity;
	class UUID;
	struct CameraComponent;

	class Scene
	{
	public:
		Scene();
		Scene(const Scene&) = default;

		Entity CreateEntity(const CString& name = CString());
		Entity CreateEntityWithUUID(UUID uuid, const CString& name = CString());

		void DestroyEntity(Entity entity);

		void OnSceneStart();
		void OnSceneStop();

		virtual void OnUpdate(float DeltaTime);

		virtual void OnViewportResize(uint32_t width, uint32_t height);

		Entity GetPrimaryCameraEntity();

		void SetPrimaryCameraComponent(CameraComponent* camera);

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

	private:
		void OnCameraComponentAdded(entt::registry& registry, entt::entity entity);

	protected:
		entt::registry SceneRegistry;

		b2World* m_PhysicsWorld = nullptr;

		CameraComponent* PrimaryCameraComponent = nullptr;

		SceneState CurrentSceneState = SceneState::Edit;

		uint32_t ViewportWidth = 0, ViewportHeight = 0;
	};
}
