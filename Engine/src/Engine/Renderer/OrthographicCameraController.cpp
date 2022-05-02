#include "depch.h"
#include "OrthographicCameraController.h"

#include "Engine/Input.h"

namespace Engine
{
	OrthographicCameraController::OrthographicCameraController(float AspectRatio)
		: AspectRatio(AspectRatio), Camera(-AspectRatio * ZoomLevel, AspectRatio * ZoomLevel, -ZoomLevel, ZoomLevel)
	{

	}

	void OrthographicCameraController::OnUpdate(Timestep DeltaTime)
	{
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_W))
			Camera.AddPosition(glm::vec3(0.f, 1.f, 0.f) * (CameraSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_A))
			Camera.AddPosition(glm::vec3(-1.f, 0.f, 0.f) * (CameraSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_S))
			Camera.AddPosition(glm::vec3(0.f, -1.f, 0.f) * (CameraSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_D))
			Camera.AddPosition(glm::vec3(1.f, 0.f, 0.f) * (CameraSpeed * DeltaTime));

		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_E))
			Camera.AddRotation(-(CameraRotationSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_Q))
			Camera.AddRotation(CameraRotationSpeed * DeltaTime);
	}

	void OrthographicCameraController::OnEvent(Event& event)
	{
		EventDispatcher d(event);
		d.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
		d.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
	}

	bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& event)
	{
		ZoomLevel -= event.GetYOffset();

		Camera.SetProjection(-AspectRatio * ZoomLevel, AspectRatio * ZoomLevel, -ZoomLevel, ZoomLevel);

		return false;
	}

	bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& event)
	{
		AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();

		Camera.SetProjection(-AspectRatio * ZoomLevel, AspectRatio * ZoomLevel, -ZoomLevel, ZoomLevel);

		return false;
	}

}