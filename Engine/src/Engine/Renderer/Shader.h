#pragma once

namespace Engine
{
	class ENGINE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Shader* Create(std::string& VertexSource, std::string& FragmentSource, bool isFilePath = false);
		static Shader* Create(const char* VertexSource, const char* FragmentSource, bool isFilePath = false);

	protected:
		std::string LoadFromFile(std::string& FilePath);

		bool LoadSource();

		std::string VertexSource;
		std::string FragmentSource;

		bool isFilePath = false;
	};
}
