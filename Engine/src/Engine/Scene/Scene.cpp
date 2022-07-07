#include "depch.h"
#include "Scene.h"

#include "Components.h"

#include "Entity.h"
#include "Engine/Renderer/Renderer2D.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

#include <glm/glm.hpp>

#include "ScriptableEntity.h"

namespace Engine
{
	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Static:
				return b2_staticBody;
			case Rigidbody2DComponent::BodyType::Dynamic:
				return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Kinematic:
				return b2_kinematicBody;
		}

		DE_CORE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	Scene::Scene()
	{
		SceneRegistry.on_construct<CameraComponent>().connect<&Scene::OnCameraComponentAdded>(this);
	}

	Entity Scene::CreateEntity(const CString& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const CString& name)
	{
		Entity entity = Entity(SceneRegistry.create(), this);
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();

		if (name.empty())
		{
			std::wstringstream sstream;

			sstream << TEXT("Entity ");
			sstream << std::to_wstring(SceneRegistry.size() - 1);

			entity.AddComponent<TagComponent>(sstream.str());
		}
		else
		{
			entity.AddComponent<TagComponent>(name);
		}

		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		SceneRegistry.destroy(entity.EntityHandle);
	}

	void Scene::OnSceneStart()
	{
		m_PhysicsWorld = new b2World({ 0.0f, -9.8f });

		auto view = SceneRegistry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, this);
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = glm::radians(transform.Rotation.z);

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnSceneStop()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		// Update Scripts
		{
			SceneRegistry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->entity = Entity(entity, this);
					nsc.Instance->OnCreate();
				}

				nsc.Instance->OnUpdate(DeltaTime);
			});
		}

		// Physics
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;
			m_PhysicsWorld->Step(DeltaTime, velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = SceneRegistry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, this);
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.Rotation.z = glm::degrees(body->GetAngle());
			}
		}

		glm::mat4 cameraTransform;
		{
			auto view = SceneRegistry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					cameraTransform = transform.GetTransformMat4();
					break;
				}
			}
		}

		if (PrimaryCameraComponent)
		{
			Renderer2D::BeginScene(PrimaryCameraComponent->Camera, cameraTransform);

			auto view = SceneRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawQuadSprite(transform.GetTransformMat4(), sprite, (int)entity);
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		ViewportWidth = width;
		ViewportHeight = height;

		auto view = SceneRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);

			if (!cameraComponent.FixedAspectRatio)
				cameraComponent.Camera.SetViewportSize(width, height);
		}
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = SceneRegistry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
				return Entity(entity, this);
		}

		return { };
	}

	void Scene::SetPrimaryCameraComponent(CameraComponent* camera)
	{
		if (camera)
			camera->Primary = true;

		if (PrimaryCameraComponent)
			PrimaryCameraComponent->Primary = false;

		PrimaryCameraComponent = camera;
	}

	void Scene::SetSceneState(SceneState state)
	{
		if (CurrentSceneState == state)
			return;

		CurrentSceneState = state;

		if (state == SceneState::Play)
			OnSceneStart();
		else if (state == SceneState::Stop)
			OnSceneStop();
	}

	void Scene::OnEvent(Event& event)
	{

	}

	void Scene::OnCameraComponentAdded(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<CameraComponent>(entity);

		if (ViewportWidth > 0 && ViewportHeight > 0)
			component.Camera.SetViewportSize(ViewportWidth, ViewportHeight);

		if (PrimaryCameraComponent == nullptr)
			SetPrimaryCameraComponent(&component);
	}
}