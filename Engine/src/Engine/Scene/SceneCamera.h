#pragma once

#include "Engine/Renderer/Camera.h"

namespace Engine
{
	class SceneCamera : public Camera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera();

		void SetOrthographic(float size, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

	private:
		void RecalculateProjection();

	private:
		float aspectRatio = 0.f;

		float OrthographicSize = 2.f;
		float OrthographicNear = -1.f;
		float OrthographicFar = 1.f;
	};
}
