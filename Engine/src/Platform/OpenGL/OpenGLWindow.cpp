#include "depch.h"
#include "OpenGLWindow.h"

#include "Engine/Renderer/GraphicsContext.h"

#include "Engine/Debug/Instrumentor.h"

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
		if (NativeWindow)
		{
			glfwDestroyWindow(NativeWindow);
			NativeWindow = nullptr;

			DE_WARN(OpenGLWindow, "Shutting down OpenGLWindow");
		}
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

		glfwSetWindowUserPointer(NativeWindow, this);
		SetVSync(true);

		glfwSetWindowSizeCallback(NativeWindow, [](GLFWwindow* window, int width, int height)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);
			wnd->Data.Width = width;
			wnd->Data.Height = height;

			WindowResizeEvent e(width, height);
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetWindowCloseCallback(NativeWindow, [](GLFWwindow* window)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e = WindowCloseEvent();
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetCharCallback(NativeWindow, [](GLFWwindow* window, unsigned int Character)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);

			KeyTypedEvent e(Character);
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetKeyCallback(NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent e(static_cast<KeyInput>(key));
				wnd->OnEventDispatch.Run(e);
			} break;
			case GLFW_RELEASE:
			{
				KeyReleasedEvent e(static_cast<KeyInput>(key));
				wnd->OnEventDispatch.Run(e);
			} break;
			case GLFW_REPEAT:
			{
				KeyPressedEvent e(static_cast<KeyInput>(key), true);
				wnd->OnEventDispatch.Run(e);
			} break;
			default:
				break;
			}
		});

		glfwSetMouseButtonCallback(NativeWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent e(static_cast<MouseButtonInput>(button));
				wnd->OnEventDispatch.Run(e);
			} break;
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent e(static_cast<MouseButtonInput>(button));
				wnd->OnEventDispatch.Run(e);
			} break;
			}
		});

		glfwSetScrollCallback(NativeWindow, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xoffset, (float)yoffset);
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetCursorPosCallback(NativeWindow, [](GLFWwindow* window, double x, double y)
		{
			OpenGLWindow* wnd = (OpenGLWindow*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x, (float)y);
			wnd->OnEventDispatch.Run(e);
		});
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

	glm::vec2 OpenGLWindow::GetFrameSize() const
	{
		int width, height;
		glfwGetFramebufferSize(NativeWindow, &width, &height);

		return { width, height };
	}

	void OpenGLWindow::SetVSync(bool Enabled)
	{
		glfwSwapInterval(1 & (int)Enabled);

		Data.VSync = Enabled;
	}
}