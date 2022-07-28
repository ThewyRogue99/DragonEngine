#pragma once

#include "Engine/Core/Core.h"
#include "EditorPanel.h"
#include "Engine/Renderer/Texture.h"

#include <filesystem>

namespace Engine
{
	class ContentBrowserPanel : public EditorPanel
	{
	public:
		ContentBrowserPanel();

		virtual void OnCreate() override;

		virtual void OnRender(float DeltaTime) override;

	private:
		void DrawDirectoryTree(const std::filesystem::path& DirectoryPath);

	private:
		std::filesystem::path CurrentDirectory;

		static Ref<Texture2D> DirectoryIcon;
		static Ref<Texture2D> FileIcon;
	};
}
