#pragma once

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

	private:
		std::string LoadFromFile(std::string& FilePath);

		bool LoadSource();

		std::string VertexSource;
		std::string FragmentSource;

		bool isFilePath = false;

		uint32_t RendererID = 0;
	};
}
