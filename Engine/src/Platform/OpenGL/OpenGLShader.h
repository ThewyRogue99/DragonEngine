#pragma once

#include "Engine/Renderer/Shader.h"

namespace Engine
{
	class OpenGLShader : public Shader
	{
	public:
		ENGINE_API OpenGLShader(const CString& VertexSource, const CString& FragmentSource, bool isFilePath = false);
		ENGINE_API ~OpenGLShader();

		ENGINE_API bool Load();

		ENGINE_API virtual void Bind() const;
		ENGINE_API virtual void Unbind() const;

		ENGINE_API virtual void SetInt(const char* name, int value) override;
		ENGINE_API virtual void SetIntArray(const char* name, int* values, size_t count) override;

		ENGINE_API virtual void SetFloat3(const char* name, const glm::vec3& value) override;
		ENGINE_API virtual void SetFloat3(const char* name, float v1, float v2, float v3) override;

		ENGINE_API virtual void SetFloat4(const char* name, const glm::vec4& value) override;
		ENGINE_API virtual void SetFloat4(const char* name, float v1, float v2, float v3, float v4) override;

		ENGINE_API virtual void SetMat4(const char* name, const glm::mat4& value) override;

	private:
		uint32_t ShaderID = 0;
	};
}