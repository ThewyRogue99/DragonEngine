#pragma once

#include <glm/glm.hpp>
#include "Engine/Renderer/Camera.h"

namespace Engine
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		SceneCamera();
		virtual ~SceneCamera();

		void SetViewportSize(uint32_t width, uint32_t height);

		// --------------------- Perspective Functions ------------------------

		void SetPerspective(float VerticalFOV, float nearClip, float farClip);

		inline float GetPerspectiveVerticalFOV() const { return glm::degrees(PerspectiveFOV); }
		inline float GetPerspectiveNearClip() const { return PerspectiveNear; }
		inline float GetPerspectiveFarClip() const { return PerspectiveFar; }

		inline void SetPerspectiveVerticalFOV(float VerticalFOV)
		{
			PerspectiveFOV = glm::radians(VerticalFOV); RecalculateProjection();
		}

		inline void SetPerspectiveNearClip(float NearClip)
		{
			PerspectiveNear = NearClip; RecalculateProjection();
		}

		inline void SetPerspectiveFarClip(float FarClip)
		{
			PerspectiveFar = FarClip; RecalculateProjection();
		}

		// --------------------- Orthographic Functions ------------------------

		void SetOrthographic(float size, float nearClip, float farClip);

		inline float GetOrthographicSize() const { return OrthographicSize; }
		inline float GetOrthographicNearClip() const { return OrthographicNear; }
		inline float GetOrthographicFarClip() const { return OrthographicFar; }

		inline void SetOrthographicSize(float Size)
		{
			OrthographicSize = Size; RecalculateProjection();
		}

		inline void SetOrthographicNearClip(float NearClip)
		{
			OrthographicNear = NearClip; RecalculateProjection();
		}

		inline void SetOrthographicFarClip(float FarClip)
		{
			OrthographicFar = FarClip; RecalculateProjection();
		}

		// ---------------------------------------------------------------------

		inline const ProjectionType GetProjectionType() const { return projectionType; }
		inline void SetProjectionType(ProjectionType type) { projectionType = type; RecalculateProjection(); }

	private:
		void RecalculateProjection();

	private:
		ProjectionType projectionType = ProjectionType::Orthographic;

		float PerspectiveFOV = glm::radians(60.f);
		float PerspectiveNear = 0.01f;
		float PerspectiveFar = 1000.f;

		float OrthographicSize = 2.f;
		float OrthographicNear = -1.f;
		float OrthographicFar = 1.f;

		float aspectRatio = 0.f;
	};
}
