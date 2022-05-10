#include "Sandbox2D.h"

Sandbox2D::Sandbox2D() : CameraController(1280.f / 720.f), Layer("Sandbox2D")
{

}

void Sandbox2D::OnAttach()
{
	VertexArray = Engine::VertexArray::Create();

	float SquareVertices[] = {
		-0.5f, -0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f
	};

	auto VertexBuffer = Engine::VertexBuffer::Create(SquareVertices, sizeof(SquareVertices));

	VertexBuffer->SetLayout({
		{ Engine::ShaderDataType::Float3, "a_Position" }
	});

	VertexArray->AddVertexBuffer(VertexBuffer);

	uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
	auto IndexBuffer = Engine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

	VertexArray->SetIndexBuffer(IndexBuffer);

	Shader = Engine::Shader::Create(
		"Shader",
		"../Engine/src/Engine/Renderer/Shaders/MaterialVertex.glsl",
		"../Engine/src/Engine/Renderer/Shaders/MaterialFragment.glsl",
		true
	);

	SquareTexture = Engine::Texture2D::Create("assets/textures/cat.jpg");
}

void Sandbox2D::OnDetach()
{

}

void Sandbox2D::OnUpdate(Engine::Timestep DeltaTime)
{
	DE_PROFILE_FUNCTION();

	{
		DE_PROFILE_SCOPE("CameraController::OnUpdate");

		CameraController.OnUpdate(DeltaTime);
	}

	{
		DE_PROFILE_SCOPE("Renderer Prep");

		Engine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Engine::RenderCommand::Clear();
	}

	{
		DE_PROFILE_SCOPE("Renderer Draw");

		Engine::Renderer2D::BeginScene(CameraController.GetCamera());

		Engine::Renderer2D::DrawQuad({ SquarePosition.x, SquarePosition.y, 0.f }, SquareSize, SquareColor);

		Engine::Renderer2D::EndScene();
	}

}

void Sandbox2D::OnImGuiRender(Engine::Timestep DeltaTime)
{
	DE_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	static float t = 0;
	t += DeltaTime;

	static unsigned int fps = 0;

	if (t > 0.5f)
	{
		fps = (unsigned int)(1 / DeltaTime);
		t = 0.f;
	}

	ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
	ImGui::DragFloat2("Square Position", glm::value_ptr(SquarePosition), 0.25f);
	ImGui::DragFloat2("Square Size", glm::value_ptr(SquareSize), 0.25f);

	ImGui::Text("FPS: %i", fps);

	ImGui::End();
}

void Sandbox2D::OnEvent(Engine::Event& event)
{
	CameraController.OnEvent(event);
}
