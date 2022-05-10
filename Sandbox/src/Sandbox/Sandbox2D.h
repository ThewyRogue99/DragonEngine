#pragma once

#include <Engine/Core/ApplicationCore.h>

#include <Engine/Renderer/OrthographicCameraController.h>

class Sandbox2D : public Engine::Layer
{
public:
	Sandbox2D();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Engine::Timestep DeltaTime) override;

	virtual void OnImGuiRender(Engine::Timestep DeltaTime) override;

	virtual void OnEvent(Engine::Event& event) override;

private:
	Engine::Ref<Engine::Shader> Shader;
	Engine::Ref<Engine::VertexArray> VertexArray;

	Engine::Ref<Engine::Texture2D> SquareTexture;

	Engine::OrthographicCameraController CameraController;

	glm::vec4 SquareColor = { 0.0f, 1.f, 1.f, 1.f };
	glm::vec2 SquarePosition = glm::vec3(0.f);
	glm::vec2 SquareSize = glm::vec3(1.f);
};

