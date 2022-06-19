#pragma once

#include <filesystem>

namespace Engine
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();

	private:
		std::filesystem::path CurrentDirectory;

		Ref<Texture2D> DirectoryIcon;
		Ref<Texture2D> FileIcon;
	};
}
