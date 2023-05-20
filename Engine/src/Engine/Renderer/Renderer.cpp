#include "depch.h"
#include "Renderer.h"

namespace Engine
{
	static Ref<RendererAPI> API_Instance = nullptr;
	static Ref<Framebuffer> ActiveFramebufferRef = nullptr;
	static RendererAPI::API m_API = RendererAPI::API::None;

	void Renderer::SetAPI(RendererAPI::API API)
	{
		m_API = API;
	}

	RendererAPI::API Renderer::GetAPI()
	{
		return m_API;
	}

	void Renderer::Init()
	{
		if (!API_Instance)
		{
			API_Instance = RendererAPI::Create(m_API);
		}
	}

	void Renderer::Shutdown()
	{
		if (API_Instance)
		{
			EndFrame();

			API_Instance = nullptr;
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
		if (API_Instance)
		{
			API_Instance->SetLineWidth(Width);
		}
	}

	void Renderer::DrawIndexed(Ref<Shader> ShaderRef, Ref<VertexArray> VertexArrayRef, uint32_t IndexCount)
	{
		if (API_Instance)
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
		if (API_Instance)
		{
			LineDrawProperties Props;
			Props.ShaderRef = ShaderRef;
			Props.FramebufferRef = ActiveFramebufferRef;
			Props.VertexArrayRef = VertexArrayRef;
			Props.VertexCount = VertexCount;

			API_Instance->DrawLine(Props);
		}
	}

	const Ref<RendererAPI>& Renderer::GetRendererAPI()
	{
		return API_Instance;
	}
}