#pragma once

#include "Engine/Renderer/Shader.h"

namespace Engine
{
	class ENGINE_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& VertexSource, const std::string& FragmentSource, bool isFilePath = false);
		~OpenGLShader();

		bool Load();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetInt(const char* name, int value) override;
		virtual void SetIntArray(const char* name, int* values, size_t count) override;

		virtual void SetFloat3(const char* name, const glm::vec3& value) override;
		virtual void SetFloat3(const char* name, float v1, float v2, float v3) override;

		virtual void SetFloat4(const char* name, const glm::vec4& value) override;
		virtual void SetFloat4(const char* name, float v1, float v2, float v3, float v4) override;

		virtual void SetMat4(const char* name, const glm::mat4& value) override;

	private:
		uint32_t ShaderID = 0;
	};
}