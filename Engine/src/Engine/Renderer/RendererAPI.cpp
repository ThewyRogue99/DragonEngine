#include "depch.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Engine
{
	RendererAPI::API RendererAPI::m_API = RendererAPI::API::None;

	RendererAPI* RendererAPI::Create(API api)
	{
		m_API = api;

		switch (m_API)
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLRendererAPI();
		default:
			return nullptr;
		}
	}
}