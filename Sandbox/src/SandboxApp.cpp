#include <Engine/ApplicationCore.h>
#include <Engine/Input.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

class SandboxLayer : public Engine::Layer
{
public:
	SandboxLayer() : Camera(-1.6f, 1.6f, -0.9f, 0.9f), Layer("SandboxLayer")
	{
		
	}

	void OnAttach() override
	{
		VertexArray.reset(Engine::VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		VertexBuffer.reset(Engine::VertexBuffer::Create(vertices, sizeof(vertices)));

		VertexBuffer->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" }
		});

		VertexArray->AddVertexBuffer(VertexBuffer);

		uint32_t indices[] = { 0, 1, 2 };
		IndexBuffer.reset(Engine::IndexBuffer::Create(indices, 3));

		VertexArray->SetIndexBuffer(IndexBuffer);

		Shader.reset(new Engine::Shader(
			"../Engine/src/Engine/Renderer/Shaders/vertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/fragment.glsl",
			true
		));

		Shader->Load();

		vArray.reset(Engine::VertexArray::Create());

		float tVertices[] = {
			-0.7f, -0.7f,  0.0f,
			 0.7f, -0.7f,  0.0f,
			 0.7f,  0.7f,  0.0f,
			-0.7f,  0.7f,  0.0f
		};

		std::shared_ptr<Engine::VertexBuffer> vBuff;
		vBuff.reset(Engine::VertexBuffer::Create(tVertices, sizeof(tVertices)));

		vBuff->SetLayout({
			{ Engine::ShaderDataType::Float3, "a_Position" }
		});

		vArray->AddVertexBuffer(vBuff);

		std::shared_ptr<Engine::IndexBuffer> iBuffer;

		uint32_t cIndices[] = { 0, 1, 2, 2, 3, 0 };
		iBuffer.reset(Engine::IndexBuffer::Create(cIndices, 6));

		vArray->SetIndexBuffer(iBuffer);

		SquareShader.reset(new Engine::Shader(
			"../Engine/src/Engine/Renderer/Shaders/vertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/fragment.glsl",
			true
		));

		SquareShader->Load();
	}

	void OnUpdate(Engine::Timestep DeltaTime) override
	{
		Engine::RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
		Engine::RenderCommand::Clear();

		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_W))
			Camera.AddPosition(glm::vec3(0.f, 1.f, 0.f) * (CameraSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_A))
			Camera.AddPosition(glm::vec3(-1.f, 0.f, 0.f ) * (CameraSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_S))
			Camera.AddPosition(glm::vec3(0.f, -1.f, 0.f ) * (CameraSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_D))
			Camera.AddPosition(glm::vec3(1.f, 0.f, 0.f ) * (CameraSpeed * DeltaTime));

		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_E))
			Camera.AddRotation(-(CameraRotationSpeed * DeltaTime));
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_Q))
			Camera.AddRotation(CameraRotationSpeed * DeltaTime);

		Engine::Renderer::BeginScene(Camera);

		static glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(0.1f));

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.15f, y * 0.15f, 0.f);
				if (x % 2 == 0)
					SquareShader->SetUniformFloat3("Color", glm::vec3(0.f, 0.f, 1.f));
				else
					SquareShader->SetUniformFloat3("Color", glm::vec3(1.f, 0.f, 0.f));

				glm::mat4 transform = glm::translate(glm::mat4(1.f), pos) * scale;

				Engine::Renderer::Submit(vArray, SquareShader, transform);
			}
		}

		Engine::Renderer::Submit(VertexArray, Shader);

		Engine::Renderer::EndScene();
	}

	void OnEvent(Engine::Event& event) override
	{

	}

private:
	std::shared_ptr<Engine::Shader> Shader;
	std::shared_ptr<Engine::Shader> SquareShader;

	std::shared_ptr<Engine::VertexArray> vArray;

	std::shared_ptr<Engine::VertexBuffer> VertexBuffer;
	std::shared_ptr<Engine::IndexBuffer> IndexBuffer;
	std::shared_ptr<Engine::VertexArray> VertexArray;

	Engine::OrthographicCamera Camera;

	float CameraSpeed = 5.f;
	float CameraRotationSpeed = 20.f;
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