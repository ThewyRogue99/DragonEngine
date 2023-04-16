#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include <glm/glm.hpp>

namespace Engine
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		ENGINE_API static Ref<Shader> Create(
			const WString& ShaderName,
			const CString& VertexSource,
			const CString& FragmentSource,
			bool isFilePath = false
		);

		inline const WString& GetName() const { return ShaderName; }

		virtual void SetInt(const char* name, int value) = 0;
		virtual void SetIntArray(const char* name, int* values, size_t count) = 0;

		virtual void SetFloat3(const char* name, const glm::vec3& value) = 0;
		virtual void SetFloat3(const char* name, float v1, float v2, float v3) = 0;

		virtual void SetFloat4(const char* name, const glm::vec4& value) = 0;
		virtual void SetFloat4(const char* name, float v1, float v2, float v3, float v4) = 0;

		virtual void SetMat4(const char* name, const glm::mat4& value) = 0;

	protected:
		ENGINE_API CString LoadFromFile(CString& FilePath);

		ENGINE_API bool LoadSource();

		CString VertexSource;
		CString FragmentSource;

		bool isFilePath = false;

	private:
		WString ShaderName;
	};
}
