#pragma once

#include "Engine/Types/Types.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/SceneCamera.h"

#include "Engine/Math/Math.h"
#include "Engine/Core/UUID.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Engine
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID id) : ID(id) { }
	};

	struct TagComponent
	{
		CString Tag;
		
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;

		TagComponent(const CString& tag)
			: Tag(tag) { }
	};

	struct TransformComponent
	{
		glm::vec3 Position = glm::vec3(0.f);
		glm::vec3 Rotation = glm::vec3(0.f);
		glm::vec3 Scale = glm::vec3(1.f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::vec3& Position)
			: Position(Position) { }

		glm::mat4 GetTransformMat4() const
		{
			glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.f);
		Ref<Texture2D> Texture = nullptr;
		float TilingFactor = 1.f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
	};

	struct CircleRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.f);
		float Radius = 0.5f;
		float Thickness = 1.f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	class ScriptableEntity;

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity* (*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

	struct Rigidbody2DComponent
	{
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;
	};

	struct BoxCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		// TODO(Yan): move into physics material in the future maybe
		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};
}