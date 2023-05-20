#include "depch.h"
#include "VulkanWindow.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

#include "Engine/Debug/Instrumentor.h"
#include "Engine/Debug/Assert.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	VulkanWindow::VulkanWindow(const WindowProps& Props)
	{
		Init(Props);
	}

	VulkanWindow::~VulkanWindow()
	{
		if (NativeWindow)
		{
			glfwDestroyWindow(NativeWindow);
			NativeWindow = nullptr;

			DE_WARN(VulkanWindow, "Shutting down VulkanWindow");
		}
	}

	void VulkanWindow::OnUpdate()
	{
		
	}

	void VulkanWindow::SetVSync(bool Enabled)
	{
		glfwSwapInterval(1 & (int)Enabled);

		Data.VSync = Enabled;
	}

	void VulkanWindow::Init(const WindowProps& Props)
	{
		DE_PROFILE_FUNCTION();

		Data.Title = Props.Title;
		Data.Width = Props.Width;
		Data.Height = Props.Height;

		DE_INFO(
			VulkanWindow, "Creating window {0}: [ Width: {1}, Height: {2} ]",
			Props.Title,
			Props.Width,
			Props.Height
		);

#ifdef ENGINE_PLATFORM_MACOS
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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

		glfwSetWindowUserPointer(NativeWindow, this);
		SetVSync(true);

		glfwSetWindowSizeCallback(NativeWindow, [](GLFWwindow* window, int width, int height)
		{
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);
			wnd->Data.Width = width;
			wnd->Data.Height = height;

			WindowResizeEvent e(width, height);
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetWindowCloseCallback(NativeWindow, [](GLFWwindow* window)
		{
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);

			WindowCloseEvent e = WindowCloseEvent();
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetCharCallback(NativeWindow, [](GLFWwindow* window, unsigned int Character)
		{
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);

			KeyTypedEvent e(Character);
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetKeyCallback(NativeWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);

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
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);

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
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xoffset, (float)yoffset);
			wnd->OnEventDispatch.Run(e);
		});

		glfwSetCursorPosCallback(NativeWindow, [](GLFWwindow* window, double x, double y)
		{
			VulkanWindow* wnd = (VulkanWindow*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)x, (float)y);
			wnd->OnEventDispatch.Run(e);
		});
	}
}