#include "depch.h"
#include "SceneCamera.h"

namespace Engine
{
	SceneCamera::SceneCamera()
	{
		
	}

	SceneCamera::~SceneCamera()
	{

	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		OrthographicSize = size;
		OrthographicNear = nearClip;
		OrthographicFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		aspectRatio = (float)width / (float)height;

		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		float orthoLeft = -OrthographicSize * aspectRatio * 0.5f;
		float orthoRight = OrthographicSize * aspectRatio * 0.5f;
		float orthoBottom = -OrthographicSize * 0.5f;
		float orthoTop = OrthographicSize * 0.5f;

		Projection = glm::ortho(
			orthoLeft,
			orthoRight,
			orthoBottom,
			orthoTop,
			OrthographicNear,
			OrthographicFar
		);
	}
}