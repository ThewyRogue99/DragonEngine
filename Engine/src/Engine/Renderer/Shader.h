#pragma once

namespace Engine
{
	class ENGINE_API Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<Shader> Create(
			const std::string& ShaderName,
			const std::string& VertexSource,
			const std::string& FragmentSource,
			bool isFilePath = false
		);

		inline const std::string& GetName() const { return ShaderName; }

	protected:
		std::string LoadFromFile(std::string& FilePath);

		bool LoadSource();

		std::string VertexSource;
		std::string FragmentSource;

		bool isFilePath = false;

	private:
		std::string ShaderName;
	};

	class ShaderLibrary
	{
	public:
		void Add(const Ref<Shader>& ShaderRef);

		Ref<Shader> Load(
			const std::string& ShaderName,
			const std::string& VertexFilePath,
			const std::string& FragmentFilePath
		);

		Ref<Shader> Get(const std::string& ShaderName);

		bool Exists(const std::string& ShaderName);

	private:
		std::unordered_map<std::string, Ref<Shader>> Shaders;
	};
}
