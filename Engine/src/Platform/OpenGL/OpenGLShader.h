#pragma once

#include "Engine/Renderer/Shader.h"

namespace Engine
{
	class ENGINE_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(std::string& VertexSource, std::string& FragmentSource, bool isFilePath = false);
		OpenGLShader(const char* VertexSource, const char* FragmentSource, bool isFilePath = false);
		~OpenGLShader();

		bool Load();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		void SetUniformFloat3(const char* name, const glm::vec3& value) const;
		void SetUniformFloat3(const char* name, float v1, float v2, float v3) const;

		void SetUniformFloat4(const char* name, const glm::vec4& value) const;
		void SetUniformFloat4(const char* name, float v1, float v2, float v3, float v4) const;

		void SetUniformMat4(const char* name, const glm::mat4& value) const;

	private:
		uint32_t ShaderID = 0;
	};
}