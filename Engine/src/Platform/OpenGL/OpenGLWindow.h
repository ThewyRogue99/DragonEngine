#pragma once

#include "Engine/Core/Window.h"

struct GLFWwindow;

namespace Engine
{
	class GraphicsContext;

	class OpenGLWindow : public Window
	{
	public:
		ENGINE_API OpenGLWindow(const WindowProps& Props);
		ENGINE_API ~OpenGLWindow();

		ENGINE_API void OnUpdate() override;

		inline unsigned int GetWidth() const override { return Data.Width; }

		inline unsigned int GetHeight() const override { return Data.Height; }

		ENGINE_API void SetVSync(bool Enabled) override;

		inline bool IsVsync() const override { return Data.VSync; }

		inline void* GetNativeWindow() const override { return NativeWindow; }

	private:
		ENGINE_API virtual void Init(const WindowProps& Props);

	private:
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