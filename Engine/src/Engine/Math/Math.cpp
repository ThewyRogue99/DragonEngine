#include "depch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Engine
{
	namespace Math
	{
		bool DecomposeTransform(const glm::mat4& transform, glm::vec3& OutPosition, glm::vec3& OutRotation, glm::vec3& OutScale)
		{
			using namespace glm;
			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], 0.f, epsilon<float>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], 0.f, epsilon<float>()) ||
				epsilonNotEqual(LocalMatrix[1][3], 0.f, epsilon<float>()) ||
				epsilonNotEqual(LocalMatrix[2][3], 0.f, epsilon<float>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = 0.f;
				LocalMatrix[3][3] = 1.f;
			}

			// Next take care of translation (easy).
			OutPosition = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3];

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			OutScale.x = length(Row[0]);// v3Length(Row[0]);
			Row[0] = detail::scale(Row[0], 1.f);

			// Now, compute Y scale and normalize 2nd row.
			OutScale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], 1.f);

			// Next, get Z scale and normalize 3rd row.
			OutScale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], 1.f);

			OutRotation.y = glm::degrees(asin(-Row[0][2]));
			if (cos(OutRotation.y) != 0.f) {
				OutRotation.x = glm::degrees(atan2(Row[1][2], Row[2][2]));
				OutRotation.z = glm::degrees(atan2(Row[0][1], Row[0][0]));
			}
			else {
				OutRotation.x = glm::degrees(atan2(-Row[2][0], Row[1][1]));
				OutRotation.z = 0.f;
			}

			return true;
		}
	}
}