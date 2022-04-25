#include "depch.h"
#include "Shader.h"

#include <glad/glad.h>
#include <fstream>

namespace Engine
{
	Shader::Shader(std::string& VertexSource, std::string& FragmentSource, bool isFilePath)
		: VertexSource(VertexSource), FragmentSource(FragmentSource), isFilePath(isFilePath) { }

	Shader::Shader(const char* VertexSource, const char* FragmentSource, bool isFilePath)
		: VertexSource(VertexSource), FragmentSource(FragmentSource), isFilePath(isFilePath) { }

	Shader::Shader()
	{

	}

	Shader::~Shader()
	{
		VertexSource.clear();
		FragmentSource.clear();

		glUseProgram(0);
		glDeleteProgram(RendererID);
	}

	bool Shader::Load(std::string& VertexSource, std::string& FragmentSource, bool isFilePath)
	{
		this->VertexSource = VertexSource;
		this->FragmentSource = FragmentSource;
		this->isFilePath = isFilePath;

		return Load();
	}

	bool Shader::Load()
	{
		if(!LoadSource()) return false;

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* source = (const GLchar*)(VertexSource.c_str());
		glShaderSource(vertexShader, 1, &source, 0);

		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			char* logStr = new char[maxLength];
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, logStr);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			DE_CORE_ASSERT(false, "Failed to compile vertex shader.\n Log: {0}", logStr);

			delete[] logStr;

			return false;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		source = (const GLchar*)(FragmentSource.c_str());
		glShaderSource(fragmentShader, 1, &source, 0);

		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			char* logStr = new char[maxLength];
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, logStr);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			DE_CORE_ASSERT(false, "Failed to compile fragment shader.\n Log: {0}", logStr);

			delete[] logStr;
			
			return false;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// 
		// Get a program object.
		RendererID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(RendererID, vertexShader);
		glAttachShader(RendererID, fragmentShader);

		// Link our program
		glLinkProgram(RendererID);

		GLint isLinked = 0;
		glGetProgramiv(RendererID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(RendererID, GL_INFO_LOG_LENGTH, &maxLength);

			char* logStr = new char[maxLength];
			glGetProgramInfoLog(RendererID, maxLength, &maxLength, logStr);

			// We don't need the program anymore.
			glDeleteProgram(RendererID);
			RendererID = 0;

			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			DE_CORE_ASSERT(false, "Failed to link shaders.\n Log: {0}", logStr);

			delete[] logStr;

			return false;
		}

		// Always detach shaders after a successful link.
		glDetachShader(RendererID, vertexShader);
		glDetachShader(RendererID, fragmentShader);

		return true;
	}

	void Shader::Bind() const
	{
		glUseProgram(RendererID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
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
