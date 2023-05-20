#include "depch.h"
#include "OpenGLRendererAPI.h"

#include "Engine/Debug/Instrumentor.h"
#include "Engine/Debug/Assert.h"

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

	OpenGLRendererAPI::OpenGLRendererAPI() : RendererAPI(API::OpenGL)
	{
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		DE_ASSERT(status, "Could not initialize GLAD!");

		DE_INFO(OpenGLContext, "OpenGL Renderer: {0} {1}", (char*)glGetString(GL_VENDOR), (char*)glGetString(GL_RENDERER));
		DE_INFO(OpenGLContext, "Vendor: {0}", (char*)glGetString(GL_VENDOR));
		DE_INFO(OpenGLContext, "Renderer: {0}", (char*)glGetString(GL_RENDERER));
		DE_INFO(OpenGLContext, "Version: {0}", (char*)glGetString(GL_VERSION));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	OpenGLRendererAPI::~OpenGLRendererAPI()
	{
		
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