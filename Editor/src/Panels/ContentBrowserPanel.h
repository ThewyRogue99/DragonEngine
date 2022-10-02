#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "EditorPanel.h"

#include "Engine/Asset/AssetManager.h"

#include <filesystem>
#include <vector>

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

		void DrawDirectoryBrowser();
		void DrawAssetBrowser();

		void DrawTopBar();

		void Reload();

		class BrowserContent
		{
		public:
			BrowserContent(ContentBrowserPanel* panel, const DirectoryEntry& entry)
				: Panel(panel), Entry(entry) { }

			void Draw(float ThumbnailSize);

			void StartRename();
			void StopRename();

		private:
			DirectoryEntry Entry;
			ContentBrowserPanel* Panel = nullptr;

			bool bShouldRename = false;

			char* RenameBuffer = nullptr;

			static const size_t RenameBufferSize = 256;
		};

	private:
		std::filesystem::path CurrentDirectory = L"";

		std::vector<BrowserContent> ContentList = { };
	};
}
