#pragma once

#include "Engine/Core/Window.h"
#include "Engine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Engine
{
	class OpenGLWindow : public Window
	{
	public:
		OpenGLWindow(const WindowProps& Props);
		virtual ~OpenGLWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth()
			const override { return Data.Width; }

		inline unsigned int GetHeight()
			const override { return Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& Callback)
			override { Data.EventCallback = Callback; }

		void SetVSync(bool Enabled) override;

		virtual void ShutDown() override;

		inline bool IsVsync() const override { return Data.VSync; }

		inline void* GetNativeWindow() const override { return NativeWindow; }

	private:
		virtual void Init(const WindowProps& Props);

		GLFWwindow* NativeWindow;

		GraphicsContext* Context;

		struct WindowData
		{
			const char* Title = nullptr;
			unsigned int Width = 1280, Height = 720;
			bool VSync = true;

			EventCallbackFn EventCallback;
		} Data;
	};
}