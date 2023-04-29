#include "depch.h"
#include "Scene.h"

#include "Engine/Debug/Instrumentor.h"

#include "Components.h"

#include "Engine/Renderer/Renderer2D.h"

#include "Engine/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>

namespace Engine
{
	Scene::Scene(const WString& Name) : SceneName(Name)
	{
		SceneRegistry.on_construct<CameraComponent>().connect<&Scene::OnCameraComponentAdded>(this);
	}

	Entity Scene::CreateEntity(const WString& name)
	{
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const WString& name)
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
		targetEntity.AddComponent<IDComponent>(UUID());

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

		PhysicsWorld.Update(DeltaTime);
		ScriptEngine::Update(DeltaTime);
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

	std::vector<unsigned int> Scene::GetRegistryVector()
	{
		std::vector<unsigned int> RegistryVector;

		SceneRegistry.each([&, &List = RegistryVector](entt::entity ent)
		{
			List.push_back((unsigned int)ent);
		});

		return RegistryVector;
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

	void Scene::OnSceneBegin()
	{
		DE_INFO(Scene, "Beginning scene '{0}'", TypeUtils::FromUTF16(SceneName).c_str());

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
				auto& sc = SceneRegistry.get<ScriptComponent>(entity);
				
				if (!sc.Name.empty())
				{
					Ref<Script> scriptObject = ScriptEngine::NewScript(sc.Namespace, sc.Name);
					if (scriptObject)
					{
						scriptObject->AttachToEntity({ entity, this });

						sc.ScriptObject = scriptObject;

						// Set Fields
						{
							for (auto& [name, data] : sc.Fields)
							{
								MemoryMap* FieldMap = (MemoryMap*)data.DataPtr;

								scriptObject->GetField(name).SetValue(FieldMap->GetFieldData("Data").DataPtr);
							}
						}
					}
				}
			}

			ScriptEngine::Run();
		}

		PhysicsWorld.Begin(this);
	}

	void Scene::OnSceneEnd()
	{
		ScriptEngine::Stop();

		PrimaryCamera = nullptr;

		PhysicsWorld.End();

		DE_INFO(Scene, "Ending scene '{0}'", TypeUtils::FromUTF16(SceneName).c_str());
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
		WString& tag = entity.GetComponent<TagComponent>().Tag;

		Entity targetEntity = TargetScene->GetEntityWithUUID(id);

		if (!targetEntity.IsValid())
			targetEntity = TargetScene->CreateEntity(tag);

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