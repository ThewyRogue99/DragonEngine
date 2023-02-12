#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Renderer/Texture.h"

#include "Editor/Project/ProjectManager.h"
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

		void OnLoadProject(const Project& project);

		void CreateContent(AssetType type);

		void ChangeCurrentDirectory(const std::filesystem::path& NewDirectory);

		class BrowserContent
		{
		public:
			BrowserContent() = default;

			BrowserContent(ContentBrowserPanel* panel, const DirectoryEntry& entry)
				: Panel(panel), Entry(entry), Type(entry.GetType()) { }

			void Draw(float ThumbnailSize, bool& DidReload);

			void DrawUncreatedContent();
			void DrawCreatedContent(bool &DidReload);

			void StartRename();
			void StopRename(bool& DidReload);

			DirectoryEntry Entry;
			ContentBrowserPanel* Panel = nullptr;

			AssetType Type = AssetType::Undefined;

			bool bShouldRename = false;
			bool bIsCreated = true;
			bool bIsError = false;

			char* RenameBuffer = nullptr;

			Ref<Texture2D> ContentIcon = nullptr;

			static const size_t RenameBufferSize = 256;
		};

		bool OnCreateContent(BrowserContent* Content);

	private:
		std::filesystem::path CurrentDirectory = L"";

		const Project* LoadedProject = nullptr;

		std::vector<BrowserContent> ContentList = { };
	};
}
