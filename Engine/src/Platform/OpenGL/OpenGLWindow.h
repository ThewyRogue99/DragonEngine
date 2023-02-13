#pragma once

#include "Engine/Core/Window.h"

struct GLFWwindow;

namespace Engine
{
	class GraphicsContext;

	class OpenGLWindow : public Window
	{
	public:
		OpenGLWindow(const WindowProps& Props);
		~OpenGLWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth()
			const override { return Data.Width; }

		inline unsigned int GetHeight()
			const override { return Data.Height; }

		void SetVSync(bool Enabled) override;

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
		} Data;
	};
}