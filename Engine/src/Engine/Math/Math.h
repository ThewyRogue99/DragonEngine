#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	namespace Math
	{
		bool DecomposeTransform(
			const glm::mat4& transform,
			glm::vec3& OutPosition,
			glm::vec3& OutRotation,
			glm::vec3& OutScale
		);
	}
}