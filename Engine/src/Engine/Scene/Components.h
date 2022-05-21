#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	struct TagComponent
	{
		std::string Tag;
		
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;

		TagComponent(const std::string& tag)
			: Tag(tag) { }
	};

	struct TransformComponent
	{
		glm::mat4 Transform = glm::mat4(1.f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;

		TransformComponent(const glm::mat4& transform)
			: Transform(transform) { }

		operator const glm::mat4& () { return Transform; }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color = glm::vec4(1.f);

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;

		SpriteRendererComponent(const glm::vec4& color)
			: Color(color) { }
	};
}