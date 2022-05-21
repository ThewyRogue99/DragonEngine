#pragma once

#include "OrthographicCamera.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() { return Right - Left; }
		float GetHeight() { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float AspectRatio);

		void OnUpdate(Timestep DeltaTime);
		void OnEvent(Event& event);

		void OnResize(float Width, float Height);

		inline OrthographicCamera& GetCamera() { return Camera; }
		inline const OrthographicCamera& GetCamera() const { return Camera; }

		const OrthographicCameraBounds& GetBounds() const { return Bounds; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& event);
		bool OnWindowResized(WindowResizeEvent& event);

	private:
		float AspectRatio;
		float ZoomLevel = 1.f;

		float CameraSpeed = 5.f;
		float CameraRotationSpeed = 20.f;

		OrthographicCameraBounds Bounds;
		OrthographicCamera Camera;
	};
}
