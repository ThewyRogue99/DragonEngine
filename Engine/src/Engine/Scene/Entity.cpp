#include "depch.h"
#include "Entity.h"

namespace Engine
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: EntityHandle(handle), CurrentScene(scene) { }
}