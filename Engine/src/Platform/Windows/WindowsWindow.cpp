#include "depch.h"

#include "WindowsWindow.h"

namespace Engine
{
	static bool bGLFWInitialized = false;

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
			DE_CORE_ASSERT(success, "Could not initialize GLFW");

			bGLFWInitialized = true;
		}

		Window = glfwCreateWindow(
			(int)Props.Width,
			(int)Props.Height,
			Data.Title.c_str(),
			nullptr,
			nullptr
		);

		glfwMakeContextCurrent(Window);
		glfwSetWindowUserPointer(Window, &Data);
		SetVSync(true);
	}

	void WindowsWindow::ShutDown()
	{
		glfwDestroyWindow(Window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(Window);
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