#pragma once

#include "OrthographicCamera.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"

namespace Engine
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float AspectRatio);

		void OnUpdate(Timestep DeltaTime);
		void OnEvent(Event& event);

		inline OrthographicCamera& GetCamera() { return Camera; }
		inline const OrthographicCamera& GetCamera() const { return Camera; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& event);
		bool OnWindowResized(WindowResizeEvent& event);

	private:
		float AspectRatio;
		float ZoomLevel = 1.f;

		float CameraSpeed = 5.f;
		float CameraRotationSpeed = 20.f;

		OrthographicCamera Camera;
	};
}
