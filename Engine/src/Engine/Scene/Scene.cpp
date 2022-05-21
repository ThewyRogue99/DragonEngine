#include "depch.h"
#include "Scene.h"

#include "Entity.h"
#include "Components.h"
#include "Engine/Renderer/Renderer2D.h"

namespace Engine
{
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { SceneRegistry.create(), this };
		entity.AddComponent<TransformComponent>();

		if (name.empty())
		{
			std::stringstream sstream;

			sstream << "Entity ";
			sstream << std::to_string(SceneRegistry.size() - 1);

			entity.AddComponent<TagComponent>(sstream.str());
		}
		else
		{
			entity.AddComponent<TagComponent>(name);
		}

		return entity;
	}

	void Scene::OnUpdate(Timestep timestep)
	{
		DE_PROFILE_FUNCTION();

		auto group = SceneRegistry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group)
		{
			auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad(transform, sprite.Color);
		}
	}
}