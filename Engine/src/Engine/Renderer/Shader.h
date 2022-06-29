#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include <glm/glm.hpp>

namespace Engine
{
	class ENGINE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<Shader> Create(
			const CString& ShaderName,
			const std::string& VertexSource,
			const std::string& FragmentSource,
			bool isFilePath = false
		);

		inline const CString& GetName() const { return ShaderName; }

		virtual void SetInt(const char* name, int value) = 0;
		virtual void SetIntArray(const char* name, int* values, size_t count) = 0;

		virtual void SetFloat3(const char* name, const glm::vec3& value) = 0;
		virtual void SetFloat3(const char* name, float v1, float v2, float v3) = 0;

		virtual void SetFloat4(const char* name, const glm::vec4& value) = 0;
		virtual void SetFloat4(const char* name, float v1, float v2, float v3, float v4) = 0;

		virtual void SetMat4(const char* name, const glm::mat4& value) = 0;

	protected:
		std::string LoadFromFile(std::string& FilePath);

		bool LoadSource();

		std::string VertexSource;
		std::string FragmentSource;

		bool isFilePath = false;

	private:
		CString ShaderName;
	};
}
