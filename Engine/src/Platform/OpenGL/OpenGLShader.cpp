#include "depch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	OpenGLShader::OpenGLShader(std::string& VertexSource, std::string& FragmentSource, bool isFilePath)
	{
		this->VertexSource = VertexSource;
		this->FragmentSource = FragmentSource;
		this->isFilePath = isFilePath;
	}

	OpenGLShader::OpenGLShader(const char* VertexSource, const char* FragmentSource, bool isFilePath)
	{
		this->VertexSource = VertexSource;
		this->FragmentSource = FragmentSource;
		this->isFilePath = isFilePath;
	}

	OpenGLShader::~OpenGLShader()
	{
		VertexSource.clear();
		FragmentSource.clear();

		glDeleteProgram(ShaderID);
		glUseProgram(0);
	}

	bool OpenGLShader::Load()
	{
		if (!LoadSource()) return false;

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
		ShaderID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(ShaderID, vertexShader);
		glAttachShader(ShaderID, fragmentShader);

		// Link our program
		glLinkProgram(ShaderID);

		GLint isLinked = 0;
		glGetProgramiv(ShaderID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(ShaderID, GL_INFO_LOG_LENGTH, &maxLength);

			char* logStr = new char[maxLength];
			glGetProgramInfoLog(ShaderID, maxLength, &maxLength, logStr);

			// We don't need the program anymore.
			glDeleteProgram(ShaderID);
			ShaderID = 0;

			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			DE_CORE_ASSERT(false, "Failed to link shaders.\n Log: {0}", logStr);

			delete[] logStr;

			return false;
		}

		// Always detach shaders after a successful link.
		glDetachShader(ShaderID, vertexShader);
		glDetachShader(ShaderID, fragmentShader);

		return true;
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(ShaderID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetUniformFloat3(const char* name, float v1, float v2, float v3) const
	{
		glUniform3f(glGetUniformLocation(ShaderID, name), v1, v2, v3);
	}

	void OpenGLShader::SetUniformFloat3(const char* name, const glm::vec3& v) const
	{
		glUniform3f(glGetUniformLocation(ShaderID, name), v.x, v.y, v.z);
	}

	void OpenGLShader::SetUniformFloat4(const char* name, float v1, float v2, float v3, float v4) const
	{
		glUniform4f(glGetUniformLocation(ShaderID, name), v1, v2, v3, v4);
	}

	void OpenGLShader::SetUniformFloat4(const char* name, const glm::vec4& v) const
	{
		glUniform4f(glGetUniformLocation(ShaderID, name), v.x, v.y, v.z, v.w);
	}

	void OpenGLShader::SetUniformMat4(const char* name, const glm::mat4& val) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ShaderID, name), 1, GL_FALSE, glm::value_ptr(val));
	}
}
