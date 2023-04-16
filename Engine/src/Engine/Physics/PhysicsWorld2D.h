#pragma once

#include "Engine/Core/Core.h"

#include <glm/glm.hpp>

#include <thread>

#include <mutex>
#include <condition_variable>

class b2World;

namespace Engine
{
	class Scene;

	class PhysicsWorld2D
	{
	public:
		PhysicsWorld2D() = default;
		ENGINE_API ~PhysicsWorld2D();

		ENGINE_API void Begin(Scene* SceneRef, const glm::vec2& Gravity = glm::vec2(0.0f, -9.8f));

		ENGINE_API void Update(float DeltaTime);

		ENGINE_API void End();

	private:
		ENGINE_API void UpdateTask();

	private:
		Scene* CurrentScene = nullptr;
		glm::vec2 WorldGravity = glm::vec2(0.f);

		b2World* b2PhysicsWorld = nullptr;

		float deltaTime = 0.f;

		bool bShouldRun = false;

		std::thread* UpdateThread = nullptr;

		std::mutex UpdateMutex;
		std::condition_variable UpdateCondition;

		std::mutex CreateBodyMutex;
	};
}