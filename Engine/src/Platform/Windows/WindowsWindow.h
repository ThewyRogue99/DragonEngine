#pragma once

#include "Engine/Window.h"

struct GLFWwindow;

namespace Engine
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& Props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth()
			const override { return Data.Width; }

		inline unsigned int GetHeight()
			const override { return Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& Callback)
			override { Data.EventCallback = Callback; }

		void SetVSync(bool Enabled) override;

		inline bool IsVsync() const override { return Data.VSync; }

	private:
		virtual void Init(const WindowProps& Props);
		virtual void ShutDown();

		GLFWwindow* Window;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData Data;
	};
}