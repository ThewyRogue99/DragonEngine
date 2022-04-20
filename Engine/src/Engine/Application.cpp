#include "depch.h"

#include "Application.h"

#include <glad/glad.h>

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

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		unsigned int indices[] = { 0, 1, 2 };
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		AppShader.reset(new Shader(
			"C:/C++/DragonEngine/Engine/src/Engine/Renderer/vertex.glsl",
			"C:/C++/DragonEngine/Engine/src/Engine/Renderer/fragment.glsl",
			true
		));

		AppShader->Load();
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
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			AppShader->Bind();
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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