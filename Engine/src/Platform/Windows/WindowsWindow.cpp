#include "depch.h"

#include "WindowsWindow.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLContext.h"

namespace Engine
{
	static bool bGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		DE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& Props)
	{
		return new WindowsWindow(Props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& Props)
	{
		Init(Props);
	}

	WindowsWindow::~WindowsWindow()
	{
		ShutDown();
	}

	void WindowsWindow::Init(const WindowProps& Props)
	{
		Data.Title = Props.Title;
		Data.Width = Props.Width;
		Data.Height = Props.Height;

		DE_CORE_INFO(
			"Creating window {0}: [ Width: {1}, Height: {2} ]",
			Props.Title,
			Props.Width,
			Props.Height
		);

		if (!bGLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			DE_CORE_ASSERT(success, "Could not initialize GLFW!");
			if (!success) return;

			glfwSetErrorCallback(GLFWErrorCallback);

			bGLFWInitialized = true;
		}

		NativeWindow = glfwCreateWindow(
			(int)Props.Width,
			(int)Props.Height,
			Data.Title.c_str(),
			nullptr,
			nullptr
		);

		Context = new OpenGLContext(NativeWindow);
		Context->Init();

		glfwSetWindowUserPointer(NativeWindow, &Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(NativeWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;
				glViewport(0, 0, width, height);

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
					KeyPressedEvent e(static_cast<KeyInput>(key), 0);
					data.EventCallback(e);
				} break;
				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(static_cast<KeyInput>(key));
					data.EventCallback(e);
				} break;
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(static_cast<KeyInput>(key), 1);
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

	void WindowsWindow::ShutDown()
	{
		if (NativeWindow)
		{
			glfwDestroyWindow(NativeWindow);
			NativeWindow = nullptr;
		}
	}

	void WindowsWindow::OnUpdate()
	{
		if (NativeWindow)
		{
			glfwPollEvents();
			Context->SwapBuffers();
		}
	}

	void WindowsWindow::SetVSync(bool Enabled)
	{
		if (Enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		Data.VSync = Enabled;
	}
}