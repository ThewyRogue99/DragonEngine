#include "depch.h"
#include "Scene.h"

#include "Components.h"

#include "Engine/Renderer/Renderer2D.h"

#include "Engine/Scripting/ScriptEngine.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include <glm/glm.hpp>

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

	Scene::Scene(const CString& Name) : SceneName(Name)
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

	Entity Scene::CopyEntity(Entity entity)
	{
		Entity targetEntity = Entity(SceneRegistry.create(), this);

		// Copy all components
		CopyComponent(AllComponents(), targetEntity, entity);

		return targetEntity;
	}

	Entity Scene::GetEntityWithUUID(UUID uuid)
	{
		auto view = SceneRegistry.view<IDComponent>();
		for (auto entity : view)
		{
			UUID id = SceneRegistry.get<IDComponent>(entity).ID;

			if (id == uuid)
				return { entity, this };
		}

		return Entity();
	}

	void Scene::DestroyEntity(Entity entity)
	{
		SceneRegistry.destroy(entity.EntityHandle);
	}

	void Scene::OnUpdate(float DeltaTime)
	{
		DE_PROFILE_FUNCTION();

		if (PrimaryCamera)
			PrimaryCamera->Update(DeltaTime);

		OnPhysics2DUpdate(DeltaTime);

		ScriptEngine::Update(DeltaTime);

		Render(DeltaTime);
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

	void Scene::OnEvent(Event& event)
	{

	}

	std::vector<Entity> Scene::GetEntities()
	{
		std::vector<Entity> EntityList;

		SceneRegistry.each([&, &List = EntityList](entt::entity ent)
		{
			List.push_back({ ent, this });
		});

		return EntityList;
	}

	void Scene::Copy(Scene* SceneRef)
	{
		SceneRegistry.each([this, SceneRef](const entt::entity entity)
		{
			CopyEntity(SceneRef, { entity, this });
		});

		SceneRef->SceneName = SceneName;
		SceneRef->ViewportHeight = ViewportHeight;
		SceneRef->ViewportWidth = ViewportWidth;
	}

	void Scene::Render(float DeltaTime)
	{
		if (PrimaryCamera)
		{
			Renderer2D::BeginScene(*PrimaryCamera, PrimaryCamera->GetTransform());

			// Sprite Rendering
			{
				auto view = SceneRegistry.view<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

					Renderer2D::DrawQuadSprite(transform.GetTransformMat4(), sprite, (int)entity);
				}
			}

			// Circle Rendering
			{
				auto view = SceneRegistry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);

					Renderer2D::DrawCircle(
						transform.GetTransformMat4(),
						circle.Color,
						circle.Thickness,
						circle.Fade,
						(int)entity
					);
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnPhysics2DStart()
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
			bodyDef.fixedRotation = rb2d.FixedRotation;

			b2Body* body = m_PhysicsWorld->CreateBody(&bodyDef);
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

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::OnPhysics2DUpdate(float DeltaTime)
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

	void Scene::OnPhysics2DEnd()
	{
		delete m_PhysicsWorld;
		m_PhysicsWorld = nullptr;
	}

	void Scene::OnSceneBegin()
	{
		// Setup Camera
		{
			auto view = SceneRegistry.view<CameraComponent, TransformComponent>();
			for (auto entity : view)
			{
				auto [camera, transform] = SceneRegistry.get<CameraComponent, TransformComponent>(entity);

				if (camera.Primary)
				{
					camera.Camera.AttachedEntity = Entity(entity, this);

					PrimaryCamera = &(camera.Camera);
				}
			}
		}

		// Setup Scripts
		{
			auto view = SceneRegistry.view<ScriptComponent>();
			for (auto entity : view)
			{
				auto& script = SceneRegistry.get<ScriptComponent>(entity);
				
				if (!script.Name.empty())
				{
					Script* scriptObject = ScriptEngine::NewScript(script.Namespace, script.Name);
					if (scriptObject)
					{
						scriptObject->AttachToEntity({ entity, this });

						script.ScriptObject = scriptObject;

						// Set Fields
						{
							for (auto& field : *(script.Fields))
							{
								field.Set(scriptObject->ScriptObject);
							}
						}
					}
				}
			}

			ScriptEngine::Run();
		}

		OnPhysics2DStart();
	}

	void Scene::OnSceneEnd()
	{
		ScriptEngine::Stop();

		PrimaryCamera = nullptr;

		OnPhysics2DEnd();
	}

	void Scene::OnCameraComponentAdded(entt::registry& registry, entt::entity entity)
	{
		auto& component = registry.get<CameraComponent>(entity);

		if (ViewportWidth > 0 && ViewportHeight > 0)
			component.Camera.SetViewportSize(ViewportWidth, ViewportHeight);
	}

	void Scene::CopyEntity(Scene* TargetScene, Entity entity)
	{
		UUID id = entity.GetComponent<IDComponent>().ID;
		CString& tag = entity.GetComponent<TagComponent>().Tag;

		Entity targetEntity = TargetScene->GetEntityWithUUID(id);

		if (!targetEntity.IsValid())
			targetEntity = TargetScene->CreateEntityWithUUID(id, tag);

		// Copy all components
		CopyComponent(AllComponents(), targetEntity, entity);
	}

	template<typename... Component>
	void Scene::CopyComponent(Entity TargetEntity, Entity SourceEntity)
	{
		([&]()
		{
			if (SourceEntity.HasComponent<Component>())
			{
				Component& sourceComponent = SourceEntity.GetComponent<Component>();

				TargetEntity.AddOrReplaceComponent<Component>(sourceComponent);
			}
		}(), ...);
	}

	template<typename... Component>
	void Scene::CopyComponent(ComponentGroup<Component...>, Entity TargetEntity, Entity SourceEntity)
	{
		CopyComponent<Component...>(TargetEntity, SourceEntity);
	}
}