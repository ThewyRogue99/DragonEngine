#pragma once

#include "Engine/Types/Types.h"

#include "Engine/Audio/AudioEngine.h"

#include "Engine/Renderer/Texture.h"
#include "Engine/Scene/SceneCamera.h"

#include "Engine/Math/Math.h"
#include "Engine/Core/UUID.h"

#include "Engine/Scripting/ScriptField.h"
#include "Engine/Scripting/Script.h"

#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define COMPONENT_CLASS_BODY() \
public: \
	friend class SceneSerializer; \
\
protected: \
	virtual void OnSerialize(AssetMetadata& Metadata) override; \
\
	virtual void OnDeserialize(AssetMetadata& Metadata) override;

#define COMPONENT_CLASS(T, ...) class T : public Component \
{ \
protected: \
	static const char* GetStaticTypeName() { return #T; } \
__VA_ARGS__ \
}

namespace Engine
{
	class AssetMetadata;

	class Component
	{
	protected:
		virtual void OnSerialize(AssetMetadata& Metadata) = 0;

		virtual void OnDeserialize(AssetMetadata& Metadata) = 0;
	};

	COMPONENT_CLASS(IDComponent,
	public:
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(const UUID& id) : ID(id) { }

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(TagComponent,
	public:
		CString Tag;
		
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;

		TagComponent(const CString& tag)
			: Tag(tag) { }

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(TransformComponent,
	public:
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

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(SpriteRendererComponent,
	public:
		glm::vec4 Color = glm::vec4(1.f);

		std::string TextureID;
		Ref<Texture2D> Texture = nullptr;

		float TilingFactor = 1.f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(CircleRendererComponent,
	public:
		glm::vec4 Color = glm::vec4(1.f);
		float Radius = 0.5f;
		float Thickness = 1.f;
		float Fade = 0.005f;

		CircleRendererComponent() = default;
		CircleRendererComponent(const CircleRendererComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(CameraComponent,
	public:
		SceneCamera Camera = SceneCamera({ });
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(AudioComponent,
	public:
		glm::vec3 Offset = glm::vec3(0.f);

		std::string AudioID;
		Ref<AudioSource> Source = nullptr;

		AudioComponent() = default;
		AudioComponent(const AudioComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(ScriptComponent,
	public:
		Script* ScriptObject;

		std::string Name = "";
		std::string Namespace = "";

		std::vector<ScriptField>* Fields = nullptr;

		ScriptComponent() = default;
		ScriptComponent(const ScriptComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(Rigidbody2DComponent,
	public:
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType Type = BodyType::Static;
		bool FixedRotation = false;

		// Storage for runtime
		void* RuntimeBody = nullptr;

		Rigidbody2DComponent() = default;
		Rigidbody2DComponent(const Rigidbody2DComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(BoxCollider2DComponent,
	public:
		glm::vec2 Offset = glm::vec2(0.0f, 0.0f);
		glm::vec2 Size = glm::vec2(0.5f, 0.5f);

		float Density = 1.0f;
		float Friction = 0.4f;
		float Restitution = 0.2f;
		float RestitutionThreshold = 1.f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		BoxCollider2DComponent() = default;
		BoxCollider2DComponent(const BoxCollider2DComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	COMPONENT_CLASS(CircleCollider2DComponent,
	public:
		glm::vec2 Offset = glm::vec2(0.0f, 0.0f);
		float Radius = 1.f;

		float Density = 1.0f;
		float Friction = 0.4f;
		float Restitution = 0.2f;
		float RestitutionThreshold = 1.f;

		// Storage for runtime
		void* RuntimeFixture = nullptr;

		CircleCollider2DComponent() = default;
		CircleCollider2DComponent(const CircleCollider2DComponent&) = default;

		COMPONENT_CLASS_BODY()
	);

	template<typename... Component>
	struct ComponentGroup
	{

	};

	using AllComponents =
		ComponentGroup<TagComponent, TransformComponent, SpriteRendererComponent,
			CircleRendererComponent, CameraComponent, ScriptComponent,
			Rigidbody2DComponent, BoxCollider2DComponent, CircleCollider2DComponent,
			AudioComponent>;
}