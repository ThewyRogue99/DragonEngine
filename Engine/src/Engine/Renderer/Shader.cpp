#include "depch.h"
#include "Shader.h"

#include <fstream>

#include "Platform/OpenGL/OpenGLShader.h"
#include "Renderer.h"

namespace Engine
{
	Shader* Shader::Create(std::string& VertexSource, std::string& FragmentSource, bool isFilePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
			{
				OpenGLShader* shader =  new OpenGLShader(VertexSource, FragmentSource, isFilePath);
				shader->Load();
				shader->Bind();

				return shader;
			}
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}

	Shader* Shader::Create(const char* VertexSource, const char* FragmentSource, bool isFilePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
			{
				OpenGLShader* shader = new OpenGLShader(VertexSource, FragmentSource, isFilePath);
				shader->Load();
				shader->Bind();

				return shader;
			}
			default:
			{
				DE_CORE_ASSERT(false, "Unknown Renderer API");
				return nullptr;
			}
		}
	}

	std::string Shader::LoadFromFile(std::string& FilePath)
	{
		std::ifstream input_file(FilePath);
		if (!input_file.is_open()) {
			return std::string();
		}

		return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	}

	bool Shader::LoadSource()
	{
		if (isFilePath)
		{
			std::string vString = LoadFromFile(VertexSource);
			if (vString.empty())
			{
				DE_CORE_ASSERT(false, "Failed to load vertex shader file from path: {0}", VertexSource.c_str());

				return false;
			}
			VertexSource = vString;
			vString.clear();

			std::string fString = LoadFromFile(FragmentSource);
			if (fString.empty())
			{
				DE_CORE_ASSERT("Failed to load fragment shader file from path: {0}", FragmentSource.c_str());

				return false;
			}
			FragmentSource = fString;
			fString.clear();
		}

		return true;
	}
}
