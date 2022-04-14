#include <Engine/ApplicationCore.h>
#include <Engine/Input.h>

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{

	}

	void OnUpdate() override
	{
		if (Engine::Input::IsKeyPressed(Engine::KeyInput::Key_Tab))
			DE_INFO("Tab pressed");
	}

	void OnEvent(Engine::Event& event) override
	{

	}
};

class Sandbox : public Engine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new Engine::ImGuiLayer());
	}

	~Sandbox()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Sandbox();
}