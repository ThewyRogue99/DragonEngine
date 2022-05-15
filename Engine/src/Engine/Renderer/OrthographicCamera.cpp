#include "depch.h"
#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.f, 1.f)), ViewProjectionMatrix(ProjectionMatrix * ViewMatrix)
	{ }

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::rotate(
			glm::translate(glm::mat4(1.f), Position),
			glm::radians(Rotation),
			glm::vec3(0.f, 0.f, 1.f)
		);

		ViewMatrix = glm::inverse(transform);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	}

}