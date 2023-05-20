#include "depch.h"
#include "Shader.h"

#include "Engine/Debug/Assert.h"

#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>

namespace Engine
{
	Ref<Shader> Shader::Create(const WString& ShaderName, const CString& VertexSource, const CString& FragmentSource, bool isFilePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
			{
				Ref<OpenGLShader> shader = std::make_shared<OpenGLShader>(VertexSource, FragmentSource, isFilePath);
				if (!shader->Load())
				{
					DE_ASSERT(false, "Failed to load shader");

					return nullptr;
				}

				shader->ShaderName = ShaderName;

				return shader;
			}
			default:
			{
				DE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}

	CString Shader::LoadFromFile(CString& FilePath)
	{
		std::ifstream input_file(FilePath);
		if (!input_file.is_open()) {
			return CString();
		}

		return CString(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>());
	}

	bool Shader::LoadSource()
	{
		if (isFilePath)
		{
			CString vString = LoadFromFile(VertexSource);
			if (vString.empty())
			{
				DE_ASSERT(false, "Failed to load vertex shader file from path: {0}", VertexSource.c_str());

				return false;
			}
			VertexSource = vString;
			vString.clear();

			CString fString = LoadFromFile(FragmentSource);
			if (fString.empty())
			{
				DE_ASSERT(false, "Failed to load fragment shader file from path: {0}", FragmentSource.c_str());

				return false;
			}
			FragmentSource = fString;
			fString.clear();
		}

		return true;
	}
}
