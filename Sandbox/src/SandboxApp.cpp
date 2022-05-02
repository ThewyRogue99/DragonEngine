#include <Engine/ApplicationCore.h>
#include <Engine/Input.h>
#include <Engine/Renderer/Texture.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <Platform/OpenGL/OpenGLShader.h>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Engine/Renderer/OrthographicCameraController.h"

class SandboxLayer : public Engine::Layer
{
public:
	SandboxLayer() : CameraController(1280.f / 720.f), Layer("SandboxLayer")
	{
		
	}

	void OnAttach() override
	{
		SquareVertexArray.reset(Engine::VertexArray::Create());

		float SquareVertices[] = {
			-0.5f, -0.5f,  0.0f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.0f,  0.0f, 1.0f
		};

		SquareVertexBuffer.reset(Engine::VertexBuffer::Create(SquareVertices, sizeof(SquareVertices)));

		SquareVertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" },
			{ Engine::ShaderDataType::Float2, "a_TexCoord"}
		});

		SquareVertexArray->AddVertexBuffer(SquareVertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
		SquareIndexBuffer.reset(Engine::IndexBuffer::Create(indices, 6));

		SquareVertexArray->SetIndexBuffer(SquareIndexBuffer);

		ShaderLib.Load(
			"SquareShader",
			"../Engine/src/Engine/Renderer/Shaders/TextureVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/TextureFragment.glsl"
		);

		GridVertexArray.reset(Engine::VertexArray::Create());

		float GridVertices[] = {
			-0.7f, -0.7f,  0.0f,
			 0.7f, -0.7f,  0.0f,
			 0.7f,  0.7f,  0.0f,
			-0.7f,  0.7f,  0.0f
		};

		GridVertexBuffer.reset(Engine::VertexBuffer::Create(GridVertices, sizeof(GridVertices)));

		GridVertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" }
		});

		GridVertexArray->AddVertexBuffer(GridVertexBuffer);

		GridIndexBuffer.reset(Engine::IndexBuffer::Create(indices, 6));

		GridVertexArray->SetIndexBuffer(GridIndexBuffer);

		ShaderLib.Load(
			"GridShader",
			"../Engine/src/Engine/Renderer/Shaders/MaterialVertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/MaterialFragment.glsl"
		);

		SquareTexture = Engine::Texture2D::Create("assets/textures/cat.jpg");
	}

	void OnUpdate(Engine::Timestep DeltaTime) override
	{
		Engine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Engine::RenderCommand::Clear();

		CameraController.OnUpdate(DeltaTime);

		Engine::Renderer::BeginScene(CameraController.GetCamera());

		static glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

		auto GridShader = ShaderLib.Get("GridShader");
		auto SquareShader = ShaderLib.Get("SquareShader");

		GridShader->Bind();
		std::dynamic_pointer_cast<Engine::OpenGLShader>(GridShader)->SetUniformFloat3("Color", GridColor);

		for (int y = -(GridSize / 2); y < (GridSize / 2); y++)
		{
			for (int x = -(GridSize / 2); x < (GridSize / 2); x++)
			{
				glm::vec3 pos(x * 0.15f + 0.075f, y * 0.15f + 0.075, 0.f);

				glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) * scale;

				Engine::Renderer::Submit(GridVertexArray, GridShader, transform);
			}
		}

		SquareTexture->Bind();
		Engine::Renderer::Submit(SquareVertexArray, SquareShader);

		Engine::Renderer::EndScene();
	}

	virtual void OnImGuiRender(Engine::Timestep DeltaTime) override
	{
		ImGui::Begin("Settings");

		static float t = 0;
		t += DeltaTime;

		static unsigned int fps = 0;

		if (t > 0.5f)
		{
			fps = (unsigned int)(1 / DeltaTime);
			t = 0.f;
		}

		ImGui::ColorEdit3("Grid Color", glm::value_ptr(GridColor));

		ImGui::SliderInt("Grid Size", &GridSize, 0, 50);

		ImGui::Text("FPS: %i", fps);

		ImGui::End();
	}

	void OnEvent(Engine::Event& event) override
	{
		CameraController.OnEvent(event);
	}

private:
	Engine::ShaderLibrary ShaderLib;

	Engine::Ref<Engine::VertexBuffer> GridVertexBuffer, SquareVertexBuffer;
	Engine::Ref<Engine::IndexBuffer> GridIndexBuffer, SquareIndexBuffer;
	Engine::Ref<Engine::VertexArray> GridVertexArray, SquareVertexArray;

	Engine::Ref<Engine::Texture2D> SquareTexture;

	Engine::OrthographicCameraController CameraController;

	glm::vec3 GridColor = glm::vec3(0.f, 0.f, 1.f);

	int GridSize = 9;
};

class Sandbox : public Engine::Application
{
public:
	Sandbox() : Engine::Application()
	{
		PushLayer(new SandboxLayer());
	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}