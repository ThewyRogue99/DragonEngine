#pragma once

#include "Engine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Engine
{
	class ENGINE_API OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* WindowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* WindowHandle = nullptr;
	};
}