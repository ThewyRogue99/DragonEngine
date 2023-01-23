#include "depch.h"

#include "OpenGLWindow.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLContext.h"

namespace Engine
{
	OpenGLWindow::OpenGLWindow(const WindowProps& Props)
	{
		Init(Props);
	}

	OpenGLWindow::~OpenGLWindow()
	{
		ShutDown();
	}

	void OpenGLWindow::Init(const WindowProps& Props)
	{
		DE_PROFILE_FUNCTION();

		Data.Title = Props.Title;
		Data.Width = Props.Width;
		Data.Height = Props.Height;

		DE_INFO(
			OpenGLWindow, "Creating window {0}: [ Width: {1}, Height: {2} ]",
			Props.Title,
			Props.Width,
			Props.Height
		);

#ifdef ENGINE_PLATFORM_MACOS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		{
			DE_PROFILE_SCOPE("Create Window");

			NativeWindow = glfwCreateWindow(
				(int)Props.Width,
				(int)Props.Height,
				Data.Title,
				nullptr,
				nullptr
			);
		}

		Context = new OpenGLContext(NativeWindow);
		Context->Init();

		glfwSetWindowUserPointer(NativeWindow, &Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(NativeWindow, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(NativeWindow, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e = WindowCloseEvent();
			data.EventCallback(e);
		});

		glfwSetCharCallback(NativeWindow, [](GLFWwindow* window, unsigned int Character)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent e(Character);
			data.EventCallback(e);
		});

		glfwSetKeyCallback(NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent e(static_cast<KeyInput>(key));
				data.EventCallback(e);
			} break;
			case GLFW_RELEASE:
			{
				KeyReleasedEvent e(static_cast<KeyInput>(key));
				data.EventCallback(e);
			} break;
			case GLFW_REPEAT:
			{
				KeyPressedEvent e(static_cast<KeyInput>(key), true);
				data.EventCallback(e);
			} break;
			}
		});

		glfwSetMouseButtonCallback(NativeWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent e(static_cast<MouseButtonInput>(button));
				data.EventCallback(e);
			} break;
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent e(static_cast<MouseButtonInput>(button));
				data.EventCallback(e);
			} break;
			}
		});

		glfwSetScrollCallback(NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xoffset, (float)yoffset);
			data.EventCallback(e);
		});

		glfwSetCursorPosCallback(NativeWindow, [](GLFWwindow* window, double x, double y)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x, (float)y);
			data.EventCallback(e);
		});
	}

	void OpenGLWindow::ShutDown()
	{
		if (NativeWindow)
		{
			glfwDestroyWindow(NativeWindow);
			NativeWindow = nullptr;

			DE_WARN(OpenGLWindow, "Shutting down OpenGLWindow");
		}
	}

	void OpenGLWindow::OnUpdate()
	{
		DE_PROFILE_FUNCTION();

		if (NativeWindow)
		{
			glfwPollEvents();
			Context->SwapBuffers();
		}
	}

	void OpenGLWindow::SetVSync(bool Enabled)
	{
		if (Enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		Data.VSync = Enabled;
	}
}