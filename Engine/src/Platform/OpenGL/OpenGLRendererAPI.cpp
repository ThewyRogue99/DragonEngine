#include "depch.h"
#include "OpenGLRendererAPI.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Engine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		DE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	void OpenGLRendererAPI::Init()
	{
		{
			DE_PROFILE_SCOPE("GLFW Init");

			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			DE_CORE_ASSERT(success, "Could not initialize GLFW!");
			if (!success) return;

			glfwSetErrorCallback(GLFWErrorCallback);
		}
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.w);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t IndexCount)
	{
		uint32_t count = IndexCount > 0 ? IndexCount : vertexArray->GetIndexBuffer()->GetCount();

		vertexArray->Bind();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t VertexCount)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, VertexCount);
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