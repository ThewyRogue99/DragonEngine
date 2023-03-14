#include "depch.h"
#include "Renderer.h"

namespace Engine
{
	static bool bIsInit = false;

	static RendererAPI* API_Instance = nullptr;
	static Ref<Framebuffer> ActiveFramebufferRef = nullptr;

	void Renderer::Init()
	{
		if (!bIsInit)
		{
			API_Instance = RendererAPI::Create(RendererAPI::API::OpenGL);

			API_Instance->Init();

			bIsInit = true;
		}
	}

	void Renderer::Shutdown()
	{
		if (bIsInit)
		{
			EndFrame();

			API_Instance->Shutdown();

			delete API_Instance;
			API_Instance = nullptr;

			bIsInit = false;
		}
	}

	void Renderer::BeginFrame(Ref<Framebuffer> FramebufferRef)
	{
		ActiveFramebufferRef = FramebufferRef;
	}

	void Renderer::EndFrame()
	{
		ActiveFramebufferRef = nullptr;
	}

	void Renderer::SetLineWidth(float Width)
	{
		if (bIsInit)
		{
			API_Instance->SetLineWidth(Width);
		}
	}

	void Renderer::DrawIndexed(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t IndexCount)
	{
		if (bIsInit)
		{
			IndexedDrawProperties Props;
			Props.ShaderRef = ShaderRef;
			Props.FramebufferRef = ActiveFramebufferRef;
			Props.VertexArrayRef = VertexArrayRef;
			Props.IndexCount = IndexCount;

			API_Instance->DrawIndexed(Props);
		}
	}

	void Renderer::DrawLine(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t VertexCount)
	{
		if (bIsInit)
		{
			LineDrawProperties Props;
			Props.ShaderRef = ShaderRef;
			Props.FramebufferRef = ActiveFramebufferRef;
			Props.VertexArrayRef = VertexArrayRef;
			Props.VertexCount = VertexCount;

			API_Instance->DrawLine(Props);
		}
	}

	RendererAPI* Renderer::GetRendererAPI()
	{
		return API_Instance;
	}
}