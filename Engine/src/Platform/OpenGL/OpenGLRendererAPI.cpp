#include "depch.h"
#include "OpenGLRendererAPI.h"

#include "Engine/Debug/Debug.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "OpenGLShader.h"
#include "OpenGLVertexArray.h"
#include "OpenGLFramebuffer.h"

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		DE_ERROR(OpenGLRendererAPI, "GLFW Error ({0}): {1}", error, description);
	}

	void OpenGLRendererAPI::Init()
	{
		{
			DE_PROFILE_SCOPE("GLFW Init");

			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			DE_ASSERT(success, "Could not initialize GLFW!");
			if (!success) return;

			glfwSetErrorCallback(GLFWErrorCallback);

			DE_INFO(OpenGLRendererAPI, "Successfully initialized OpenGLRendererAPI");
		}
	}

	void OpenGLRendererAPI::Shutdown()
	{
		glfwTerminate();
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.w);
	}

	void OpenGLRendererAPI::Clear(Ref<Framebuffer> FramebufferRef)
	{
		std::dynamic_pointer_cast<OpenGLFramebuffer>(FramebufferRef)->Bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::dynamic_pointer_cast<OpenGLFramebuffer>(FramebufferRef)->Unbind();
	}

	void OpenGLRendererAPI::DrawIndexed(const IndexedDrawProperties& Props)
	{
		std::dynamic_pointer_cast<OpenGLVertexArray>(Props.VertexArrayRef)->Bind();
		std::dynamic_pointer_cast<OpenGLFramebuffer>(Props.FramebufferRef)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(Props.ShaderRef)->Bind();

		uint32_t count = Props.IndexCount > 0 ? Props.IndexCount : Props.VertexArrayRef->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

		std::dynamic_pointer_cast<OpenGLVertexArray>(Props.VertexArrayRef)->Unbind();
		std::dynamic_pointer_cast<OpenGLFramebuffer>(Props.FramebufferRef)->Unbind();
		std::dynamic_pointer_cast<OpenGLShader>(Props.ShaderRef)->Unbind();
	}

	void OpenGLRendererAPI::DrawLine(const LineDrawProperties& Props)
	{
		std::dynamic_pointer_cast<OpenGLVertexArray>(Props.VertexArrayRef)->Bind();
		std::dynamic_pointer_cast<OpenGLFramebuffer>(Props.FramebufferRef)->Bind();
		std::dynamic_pointer_cast<OpenGLShader>(Props.ShaderRef)->Bind();

		glDrawArrays(GL_LINES, 0, Props.VertexCount);

		std::dynamic_pointer_cast<OpenGLVertexArray>(Props.VertexArrayRef)->Unbind();
		std::dynamic_pointer_cast<OpenGLFramebuffer>(Props.FramebufferRef)->Unbind();
		std::dynamic_pointer_cast<OpenGLShader>(Props.ShaderRef)->Unbind();
	}

	void OpenGLRendererAPI::SetLineWidth(float Width)
	{
		glLineWidth(Width);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}
}