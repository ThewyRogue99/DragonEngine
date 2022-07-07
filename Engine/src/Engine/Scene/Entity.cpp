#include "depch.h"
#include "Entity.h"

#include "Components.h"

namespace Engine
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: EntityHandle(handle), CurrentScene(scene) { }

	bool Entity::IsValid()
	{
		if (EntityHandle == entt::null)
			return false;
		else if (CurrentScene == nullptr)
			return false;
		else if (!(CurrentScene->SceneRegistry.valid(EntityHandle)))
			return false;

		return true;
	}
}