#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class ENGINE_API Shader
	{
	public:
		Shader(std::string& VertexSource, std::string& FragmentSource, bool isFilePath = false);
		Shader(const char* VertexSource, const char* FragmentSource, bool isFilePath = false);
		Shader();
		~Shader();

		bool Load(std::string& VertexSource, std::string& FragmentSource, bool isFilePath = false);
		bool Load();

		void Bind() const;
		void Unbind() const;

		void SetUniformFloat3(const char* name, const glm::vec3& value);

		void SetUniformFloat4(const char* name, const glm::vec4& value);
		void SetUniformMat4(const char* name, const glm::mat4& value);

	private:
		std::string LoadFromFile(std::string& FilePath);

		bool LoadSource();

		std::string VertexSource;
		std::string FragmentSource;

		bool isFilePath = false;

		uint32_t RendererID = 0;
	};
}
