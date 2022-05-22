#pragma once

#include "Entity.h"

namespace Engine
{
	class ScriptableEntity
	{
	public:
		virtual ~ScriptableEntity() { }

		template<typename T>
		inline T& GetComponent()
		{
			return entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() { }
		virtual void OnDestroy() { }
		virtual void OnUpdate(Timestep DeltaTime) { }

	private:
		Entity entity;

		friend class Scene;
	};
}