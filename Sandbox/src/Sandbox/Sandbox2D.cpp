#include "Sandbox2D.h"

#include <imgui.h>

Sandbox2D::Sandbox2D() : CameraController(1280.f / 720.f), Layer("Sandbox2D")
{

}

void Sandbox2D::OnAttach()
{
	Engine::FramebufferSpecification Props;
	Props.Width = 1280;
	Props.Height = 720;

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

	particleProps.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	particleProps.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	particleProps.SizeBegin = 0.5f, particleProps.SizeVariation = 0.3f, particleProps.SizeEnd = 0.0f;
	particleProps.LifeTime = 2.5f;
	particleProps.Velocity = { 0.0f, 0.0f };
	particleProps.VelocityVariation = { 3.0f, 1.0f };
	particleProps.Position = { 0.0f, 0.0f };
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

	Engine::Renderer2D::ResetStats();
	{
		DE_PROFILE_SCOPE("Renderer Prep");

		Engine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Engine::RenderCommand::Clear();
	}

	{
		DE_PROFILE_SCOPE("Renderer Draw");

		Engine::Renderer2D::BeginScene(CameraController.GetCamera());

		static float rotation = 0.f;

		rotation += DeltaTime * SquareRotationSpeed * 10.f;

		Engine::Renderer2D::RenderProperties props;
		props.TilingFactor = 10.f;

		Engine::Renderer2D::DrawQuad(glm::vec3(0.f, 0.f, -0.1f), glm::vec2(20.f), SquareTexture, props);

		Engine::Renderer2D::DrawRotatedQuad({ SquarePosition.x, SquarePosition.y, 0.f }, rotation, SquareSize, SquareColor);
		Engine::Renderer2D::DrawQuad(glm::vec3(2.f, 0.f, 0.f), glm::vec2(1.f), glm::vec4(1.f, 0.f, 0.f, 1.f));
		Engine::Renderer2D::DrawRotatedQuad(glm::vec3(2.f, -1.f, 0.f), 45.f, glm::vec2(1.f), glm::vec4(0.f, 0.f, 1.f, 1.f));
		Engine::Renderer2D::DrawQuad(glm::vec3(-2.f, 0.f, 0.f), glm::vec2(1.f), glm::vec4(1.f, 0.f, 0.f, 1.f));

		Engine::Renderer2D::EndScene();

		Engine::Renderer2D::BeginScene(CameraController.GetCamera());

		for (float y = -5.f; y < 5.f; y += 0.5f)
		{
			for (float x = -5.f; x < 5.f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.f) / 10.f, 0.4f, (y + 5.f) / 10.f, 0.7f };
				Engine::Renderer2D::DrawQuad({ x, y, 0.f }, { 0.45f, 0.45f}, color);
			}
		}

		Engine::Renderer2D::EndScene();
	}

	if (Engine::Input::IsMouseButtonPressed(Engine::MouseButtonInput::MouseButton_Left))
	{
		auto [x, y] = Engine::Input::GetMousePosition();
		auto width = Engine::Application::Get().GetWindow().GetWidth();
		auto height = Engine::Application::Get().GetWindow().GetHeight();

		auto bounds = CameraController.GetBounds();
		auto pos = CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		particleProps.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			particleSystem.Emit(particleProps);
	}

	particleSystem.OnUpdate(DeltaTime);
	particleSystem.OnRender(CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender(Engine::Timestep DeltaTime)
{
	DE_PROFILE_FUNCTION();

	static float t = 0;
	t += DeltaTime;

	static unsigned int fps = 0;

	if (t > 0.5f)
	{
		fps = (unsigned int)(1 / DeltaTime);
		t = 0.f;
	}

	auto stats = Engine::Renderer2D::GetStats();

	ImGui::Begin("Settings");

	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %i", stats.DrawCalls);
	ImGui::Text("Quad Count: %i", stats.QuadCount);
	ImGui::Text("Vertex Count: %i", stats.GetTotalVertexCount());
	ImGui::Text("Index Count: %i", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("Square Color", glm::value_ptr(SquareColor));
	ImGui::DragFloat2("Square Position", glm::value_ptr(SquarePosition), 0.1f);
	ImGui::DragFloat2("Square Size", glm::value_ptr(SquareSize), 0.1f);
	ImGui::DragFloat("Square Rotation Speed", &SquareRotationSpeed, 0.05f, 0.f, 10.f);

	ImGui::Text("FPS: %i", fps);

	ImGui::End();
}

void Sandbox2D::OnEvent(Engine::Event& event)
{
	CameraController.OnEvent(event);
}
