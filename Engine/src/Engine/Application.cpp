#include "depch.h"

#include "Application.h"

#include "Renderer/Renderer.h"

namespace Engine
{
	Application* Application::Instance = nullptr;

	Application::Application()
	{
		DE_CORE_ASSERT(!Instance, "Application already exists");
		Instance = this;

		AppWindow = std::unique_ptr<Window>(Window::Create());
		AppWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		AppImGuiLayer = new ImGuiLayer();
		PushOverlay(AppImGuiLayer);

		AppVertexArray.reset(VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,  1.f, 0.f, 0.f,
			 0.5f, -0.5f, 0.0f,  0.f, 0.f, 1.f,
			 0.0f,  0.5f, 0.0f,  0.f, 1.f, 0.f
		};

		AppVertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		AppVertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Color" }
		});

		AppVertexArray->AddVertexBuffer(AppVertexBuffer);

		uint32_t indices[] = { 0, 1, 2 };
		AppIndexBuffer.reset(IndexBuffer::Create(indices, 3));

		AppVertexArray->SetIndexBuffer(AppIndexBuffer);

		AppShader.reset(new Shader(
			"../Engine/src/Engine/Renderer/Shaders/vertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/fragment.glsl",
			true
		));

		AppShader->Load();

		vArray.reset(VertexArray::Create());

		float tVertices[] = {
			-0.7f, -0.7f,  0.0f,  0.f, 0.f, 1.f,
			 0.7f, -0.7f,  0.0f,  0.f, 0.f, 1.f,
			 0.7f,  0.7f,  0.0f,  0.f, 0.f, 0.2f,
			-0.7f,  0.7f,  0.0f,  0.f, 0.f, 0.2f
		};

		std::shared_ptr<VertexBuffer> vBuff;
		vBuff.reset(VertexBuffer::Create(tVertices, sizeof(tVertices)));

		vBuff->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Color" }
		});

		vArray->AddVertexBuffer(vBuff);

		std::shared_ptr<IndexBuffer> iBuffer;

		uint32_t cIndices[] = { 0, 1, 2, 2, 3, 0 };
		iBuffer.reset(IndexBuffer::Create(cIndices, 6));

		vArray->SetIndexBuffer(iBuffer);

		CubeShader.reset(new Shader(
			"../Engine/src/Engine/Renderer/Shaders/vertex.glsl",
			"../Engine/src/Engine/Renderer/Shaders/fragment.glsl",
			true
		));

		CubeShader->Load();
	}

	Application::~Application()
	{
		
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		layerStack.PushOverlay(layer);
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher d(event);
		d.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			(*--it)->OnEvent(event);

			if (event.IsHandled())
				break;
		}
	}

	void Application::Run()
	{
		bIsRunning = true;

		while (bIsRunning)
		{
			RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1));
			RenderCommand::Clear();

			Renderer::BeginScene();

			CubeShader->Bind();
			Renderer::Submit(vArray);

			AppShader->Bind();
			Renderer::Submit(AppVertexArray);

			Renderer::EndScene();

			for (Layer* layer : layerStack)
				layer->OnUpdate();

			AppImGuiLayer->Begin();

			for (Layer* layer : layerStack)
				layer->OnImGuiRender();

			AppImGuiLayer->End();

			AppWindow->OnUpdate();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& event)
	{
		bIsRunning = false;

		return true;
	}
}