#include "depch.h"
#include "Shader.h"

#include "Engine/Core/Log.h"

#include <fstream>

#include "Platform/OpenGL/OpenGLShader.h"

namespace Engine
{
	Ref<Shader> Shader::Create(const std::string& ShaderName, const std::string& VertexSource, const std::string& FragmentSource, bool isFilePath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::OpenGL:
			{
				Ref<OpenGLShader> shader = std::make_shared<OpenGLShader>(VertexSource, FragmentSource, isFilePath);
				if (!shader->Load())
				{
					DE_CORE_ASSERT(false, "Failed to load shader");

					return nullptr;
				}

				shader->ShaderName = ShaderName;
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

		return std::string(std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>());
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
				DE_CORE_ASSERT(false, "Failed to load fragment shader file from path: {0}", FragmentSource.c_str());

				return false;
			}
			FragmentSource = fString;
			fString.clear();
		}

		return true;
	}

	void ShaderLibrary::Add(const Ref<Shader>& ShaderRef)
	{
		auto& name = ShaderRef->GetName();
		DE_CORE_ASSERT(!name.empty(), "Shader name is required to add to shader library");

		bool exists = Exists(name);
		DE_CORE_ASSERT(!exists, "Shader with provided name already exists");

		if(!exists) Shaders[name] = ShaderRef;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& ShaderName, const std::string& VertexFilePath, const std::string& FragmentFilePath)
	{
		auto shader = Shader::Create(ShaderName, VertexFilePath, FragmentFilePath, true);
		Add(shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& ShaderName)
	{
		bool exists = Exists(ShaderName);
		DE_CORE_ASSERT(exists, "Shader not found!");

		return (exists ? Shaders[ShaderName] : nullptr);
	}

	bool ShaderLibrary::Exists(const std::string& ShaderName)
	{
		return Shaders.find(ShaderName) != Shaders.end();
	}
}
