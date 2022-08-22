#pragma once

#include "Engine/Types/Types.h"
#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/SceneCamera.h"

#include "Engine/Math/Math.h"
#include "Engine/Core/UUID.h"

#include "Engine/Scripting/Script.h"

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

		TransformComponent(const glm::mat4& Transform)
		{
			Math::DecomposeTransform(Transform, Position, Rotation, Scale);
		}

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
		SceneCamera Camera = SceneCamera({ });
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		Script* ScriptObject;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;
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

		float Density = 1.0f;
		float Friction = 0.4f;
		float Restitution = 0.2f;
		float RestitutionThreshold = 1.f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;
	};

	struct CircleCollider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		float Radius = 0.5f;

		float Density = 1.0f;
		float Friction = 0.4f;
		float Restitution = 0.2f;
		float RestitutionThreshold = 1.f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;
	};

	template<typename... Component>
	struct ComponentGroup
	{

	};

	using AllComponents =
		ComponentGroup<TagComponent, TransformComponent, SpriteRendererComponent,
			CircleRendererComponent, CameraComponent, ScriptComponent,
			Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent>;
}