#include "depch.h"
#include "OpenGLInput.h"

#include "Engine/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Engine
{
	Input* Input::Instance = new OpenGLInput();

	bool OpenGLInput::IsKeyPressedImpl(KeyInput KeyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int state = glfwGetKey(window, static_cast<int>(KeyCode));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool OpenGLInput::IsMouseButtonPressedImpl(MouseButtonInput Button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		int state = glfwGetMouseButton(window, static_cast<int>(Button));

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	std::pair<float, float> OpenGLInput::GetMousePositionImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float OpenGLInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();

		return x;
	}
	float OpenGLInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();

		return y;
	}
}